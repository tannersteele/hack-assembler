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
		const std::vector<std::string> commands =
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

		// RAM[0] => stack pointer (this addr contains mem addr containing topmost stack value)
		// RAM[16]->RAM[255] => static variables
		// RAM[256]->RAM[2047] => stack
		// "Push constant" doesn't fetch a variable from anywhere, just does the address trick

		// pop -> SP-- followed by x = RAM[SP]
		// add, sub, eq, gt, lt have 2 implicit operands (all supported by alu - will be similar to add operation below except last instruction will change!)

		// Add example, could probably optimize?
		// @SP (top of stack)
		// M=M-1 (move to the first register we're processing)
		// A=M (set A register to that register we want to process)
		// D=M (actually fetch the value inside the register)
		// M=0 (set the register to 0 since we're 'popping' it) - apparently this doesn't happen given the VM translator example
		// @SP (reset our A register to stack pointer)
		// A=M-1 (move to 2nd argument in add expression)
		// M=D+M (add x + y then store result in pre-existing register)

		for (const auto& command : commands)
		{
			std::string cmd = command;
			current_command_type_ = get_command_type(cmd);

			if (current_command_type_ == c_arithmetic)
				continue;

			std::string first_arg = get_first_arg(cmd);
			get_second_arg(cmd);
			std::string second_arg = cmd;
		}
	}

	static bool check_operation_modify_command(std::string& command, const std::string& operation)
	{
		if (command.find(operation) == 0)
		{
			command.erase(0, operation.length());
			return true;
		}

		return false;
	}

	static command_type get_command_type(std::string& command)
	{
		if (command == "eq" || command == "lt" || command == "gt" || command == "and" || command == "or" || command == "not" || command == "add" || command == "neg")
			return c_arithmetic;

		if (check_operation_modify_command(command, "push "))
			return c_push;

		if (check_operation_modify_command(command, "pop "))
			return c_pop;

		return none;
	}

	static void parse_substring(std::string& original_string, bool trim_after_space);

	[[nodiscard]] static std::string get_first_arg(std::string command) // copy here, get_second_arg still needs full string (for now!)
	{
		parse_substring(command, false);
		return command;
	}

	static void get_second_arg(std::string& command) // no copy needed, mangle the string
	{
		parse_substring(command, true);
	}
};

void parser::parse_substring(std::string& original_string, const bool trim_after_space)
{
	const size_t pos = original_string.find(' ');
	if (pos != std::string::npos)
		original_string = original_string.substr(trim_after_space ? pos + 1 : 0, trim_after_space ? original_string.length() : pos);
}

class code_writer
{
public:
	code_writer()
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
