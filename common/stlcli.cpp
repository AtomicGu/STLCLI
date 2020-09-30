/*
 * Copyright (c) 2019 by Yuhao Gu. All rights reserved.
 * E-Mail: yhgu2000@outlook.com
v1.1.1 */

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
	for (auto i : con._cmdMap)
	{
		con.width(20);
		con << i.first << '\t' << i.second._remarks << std::endl;
	}
	con.unlock();
}

void stlcli::default_cmd_exit_proc(std::istream& argi, stlcli::Console& con)
{
	int mainReturn = 0;
	argi >> mainReturn;
	con.end(mainReturn);
}

void stlcli::default_cmd_clear_proc(std::istream& argi, stlcli::Console& con)
{
	con << "\033[2J" << std::flush;
}

struct EndOfMain
{
	int _returnValue;
};

stlcli::Console::Console(
	const CmdMap& cmdMap,
	std::istream& in,
	std::ostream& out,
	WrongCmdProc wrongCmdProc,
	BlankCmdProc blankCmdProc,
	ArgumentErrorProc argumentErrorProc
) :
	std::ostream(out.rdbuf()),
	_wrongCmdProc(wrongCmdProc),
	_blankCmdProc(blankCmdProc),
	_argumentErrorProc(argumentErrorProc),
	_in(in),
	_cmdMap(cmdMap)
{}

stlcli::Console::Console(
	CmdMap&& cmdMap,
	std::istream& in,
	std::ostream& out,
	WrongCmdProc wrongCmdProc,
	BlankCmdProc blankCmdProc,
	ArgumentErrorProc argumentErrorProc
) :
	std::ostream(out.rdbuf()),
	_wrongCmdProc(wrongCmdProc),
	_blankCmdProc(blankCmdProc),
	_argumentErrorProc(argumentErrorProc),
	_in(in),
	_cmdMap(std::move(cmdMap))
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
			_blankCmdProc(*this);
			continue;
		}
		try
		{
			if (_cmdMap.count(cmd))
			{
				_cmdMap.at(cmd)._cmdProc(line_in, *this);
				continue;
			}
			_wrongCmdProc(cmd, *this);
		}
		catch (EndOfMain & e)
		{
			return e._returnValue;
		}
		catch (err::ArgumentError & e)
		{
			_argumentErrorProc(e, *this);
			continue;
		}
	}
	return 0;
}

void stlcli::Console::end(int main_return)
{
	throw EndOfMain{ main_return };
}
