/*
 * Copyright (c) 2019 by Yuhao Gu. All rights reserved.
 * E-Mail: yhgu2000@outlook.com
v1.1.1 */

#pragma once
#include <iostream>
#include <mutex>
#include <map>
#include <stdexcept>

namespace stlcli
{
	namespace err
	{
		/* argument error types */
		typedef std::invalid_argument ArgumentError;
	}

	/* forehead statement */
	class Console;

	/* default proccess functions */
	void default_wrong_cmd_proc(const std::string& cmd, Console& con);
	void default_blank_cmd_proc(Console& con);
	void default_argument_error_proc(err::ArgumentError& e, Console& con);
	void default_cmd_help_proc(std::istream& argi, Console& con);
	void default_cmd_exit_proc(std::istream& argi, Console& con);
	void default_cmd_clear_proc(std::istream& argi, Console& con);

	/* console used function types */
	typedef void (*CmdProc)(std::istream& argi, Console& con);
	typedef void (*WrongCmdProc)(const std::string& cmd, Console& con);
	typedef void (*BlankCmdProc)(Console& con);
	typedef void (*ArgumentErrorProc)(err::ArgumentError& e, Console& con);

	/* command type */
	struct Cmd
	{
		CmdProc _cmdProc;
		std::string _remarks;
	};

	/* command map type */
	typedef std::map<std::string, Cmd> CmdMap;

	/* class console */
	class Console :public std::ostream, public std::mutex
	{
	public:
		WrongCmdProc _wrongCmdProc;
		BlankCmdProc _blankCmdProc;
		ArgumentErrorProc _argumentErrorProc;
		std::istream& _in;
		CmdMap _cmdMap;

	public:
		Console(
			const CmdMap& cmdMap,
			std::istream& in = std::cin,
			std::ostream& out = std::cout,
			WrongCmdProc wrongCmdProc = default_wrong_cmd_proc,
			BlankCmdProc blankCmdProc = default_blank_cmd_proc,
			ArgumentErrorProc argumentErrorProc = default_argument_error_proc
		);

		Console(
			CmdMap&& cmdMap,
			std::istream& in = std::cin,
			std::ostream& out = std::cout,
			WrongCmdProc wrongCmdProc = default_wrong_cmd_proc,
			BlankCmdProc blankCmdProc = default_blank_cmd_proc,
			ArgumentErrorProc argumentErrorProc = default_argument_error_proc
		);

	public:
		int main();
		void end(int mainReturn);
	};
}
