#include "stlcli.h"
#include <string>
#include <sstream>

void stlcli::default_wrong_cmd_proc(const std::string& cmd, stlcli::Console& con)
{
	con.lock();
	con << cmd << ": command not found" << std::endl;
	con.unlock();
}

void stlcli::default_blank_cmd_proc(stlcli::Console& con)
{
}

void stlcli::default_argument_error_proc(err::ArgumentError& e, stlcli::Console& con)
{
	con.lock();
	con << "Argument Error: " << e.what() << std::endl;
	con.unlock();
}

void stlcli::default_cmd_help_proc(std::istream& args_in, stlcli::Console& con)
{
	con.lock();
	con.width(20);
	con << "Command Line" << "\tDescription\n\n";
	for (auto i : con._cmd_map)
	{
		con.width(20);
		con << i.first << '\t' << i.second._remarks << std::endl;
	}
	con.unlock();
}

void stlcli::default_cmd_exit_proc(std::istream& args_in, stlcli::Console& con)
{
	int main_return = 0;
	args_in >> main_return;
	con.end(main_return);
}

void stlcli::default_cmd_clear_proc(std::istream& argi, stlcli::Console& con)
{
	con << "\033[2J" << std::flush;
}

struct EndOfMain { int _return_code; };

stlcli::Console::Console(
	const CmdMap& cmd_map,
	std::istream& in,
	std::ostream& out,
	pfWrongCmdProc wrong_cmd_proc_pf,
	pfBlankCmdProc blank_cmd_proc_pf,
	pfArgumentErrorProc argument_error_proc_pf
) :
	std::ostream(out.rdbuf()),
	_wrong_cmd_proc_pf(wrong_cmd_proc_pf),
	_blank_cmd_proc_pf(blank_cmd_proc_pf),
	_argument_error_proc_pf(argument_error_proc_pf),
	_in(in),
	_cmd_map(cmd_map)
{}

stlcli::Console::Console(
	CmdMap&& cmd_map,
	std::istream& in,
	std::ostream& out,
	pfWrongCmdProc wrong_cmd_proc_pf,
	pfBlankCmdProc blank_cmd_proc_pf,
	pfArgumentErrorProc argument_error_proc_pf
) :
	std::ostream(out.rdbuf()),
	_wrong_cmd_proc_pf(wrong_cmd_proc_pf),
	_blank_cmd_proc_pf(blank_cmd_proc_pf),
	_argument_error_proc_pf(argument_error_proc_pf),
	_in(in),
	_cmd_map(std::move(cmd_map))
{}

int stlcli::Console::main()
{
	std::string line;
	while (std::getline(_in, line))
	{
		this->lock();
		*this << '>';
		std::getline(_in, line);
		this->unlock();
		std::istringstream line_in(line);
		std::string cmd;
		line_in >> cmd;
		if (cmd == "")
		{
			_blank_cmd_proc_pf(*this);
			continue;
		}
		try
		{
			if (_cmd_map.count(cmd))
			{
				_cmd_map.at(cmd)._cmd_proc(line_in, *this);
				continue;
			}
			_wrong_cmd_proc_pf(cmd, *this);
		}
		catch (EndOfMain & e)
		{
			return e._return_code;
		}
		catch (err::ArgumentError & e)
		{
			_argument_error_proc_pf(e, *this);
			continue;
		}
	}
	return 0;
}

void stlcli::Console::end(int main_return)
{
	throw EndOfMain{ main_return };
}
