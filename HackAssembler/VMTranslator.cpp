#include <string>
#include <vector>
#include <iostream>
/*
	Test stuff - WIP
*/
enum command_type
{
	none			= 0,
	c_arithmetic		= 1 << 0,
	c_push			= 1 << 1,
	c_pop			= 1 << 2,
	c_label			= 1 << 3,
	c_goto			= 1 << 4,
	c_if			= 1 << 5,
	c_function		= 1 << 6,
	c_return		= 1 << 7,
	c_call			= 1 << 8
};

class parser
{
private:
	command_type current_command_type_;
	std::string current_command_;

public:
	parser(): current_command_type_()
	{
		// Some test commands..
		std::vector<std::string> commands =
		{
			"push constant 111", "push constant 333", "push constant 888", "pop static 8", "add"
		};
		// Push constant 111 will produce: 
		// @111
		// D=A
		// @SP (starts at 256)
		// A=M
		// M=D
		// @SP
		// M=M+1

		for (const auto& command : commands)
		{
			std::cout << command << '\n';
		}
	}

	command_type get_command_type()
	{
	}

	std::string get_first_arg(const std::string& command)
	{
		if (current_command_type_ == command_type::c_return)
			return "";

		size_t pos = command.find(' ');
		return pos != std::string::npos ?
			command.substr(0, pos) :
			command;
	}

	std::string get_second_arg()
	{
	}
};

class CodeWriter
{
public:
	CodeWriter()
	{
	}

	void write_arithmetic(std::string command)
	{
	}

	void write_push_pop(std::string command, std::string segment, int32_t idx)
	{
	}
};

int main()
{
	parser p;
	return 0;
}
