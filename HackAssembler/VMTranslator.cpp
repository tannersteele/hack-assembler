#include <format>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
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

// Always compare a value being parsed + processed against these boundaries
constexpr int32_t virtual_register_address_space = 0x000;
constexpr int32_t static_address_space = 0x010;
constexpr int32_t stack_address_space_start = 0x100;
constexpr int32_t stack_address_space_end = 0x7FF;


class parser
{
private:
	command_type current_command_type_;
	std::string current_command_;

public:
	parser(): current_command_type_()
	{
		// Some test commands..
		const std::vector<std::string> commands = {
			"push constant 17",
			"push constant 17",
			"eq",
			"push constant 17",
			"push constant 16",
			"eq",
			"push constant 16",
			"push constant 17",
			"eq",
			"push constant 892",
			"push constant 891",
			"lt",
			"push constant 891",
			"push constant 892",
			"lt",
			"push constant 891",
			"push constant 891",
			"lt",
			"push constant 32767",
			"push constant 32766",
			"gt",
			"push constant 32766",
			"push constant 32767",
			"gt",
			"push constant 32766",
			"push constant 32766",
			"gt",
			"push constant 57",
			"push constant 31",
			"push constant 53",
			"add",
			"push constant 112",
			"sub",
			//"neg",
			//"and",
			//"push constant 82",
			//"or",
			//"not"
		};

		std::unordered_map<std::string, std::string> equality_to_asm_command =
		{
			{"eq", "D;JEQ"},
			{"gt", "D;JGT"},
			{"lt", "D;JLT"}
		};

		// pop -> SP-- followed by x = RAM[SP]
		// add, sub, eq, gt, lt have 2 implicit operands (all supported by alu - will be similar to add operation below except last instruction will change!)

		std::vector<std::string> temp_asm_arr; //just a hack for testing

		uint32_t arithmetic_operation_counter = 0;
		for (const auto& command : commands)
		{
			std::string cmd = command;
			current_command_type_ = get_command_type(cmd);

			if (current_command_type_ != c_arithmetic)
			{
				std::string first_arg = get_first_arg(cmd);
				get_second_arg(cmd);
				std::string second_arg = cmd;

				temp_asm_arr.emplace_back(HACK_get_pop_push_asm(first_arg, std::stoi(second_arg)));
			}
			else
			{
				temp_asm_arr.emplace_back(HACK_get_arithmetic_asm(cmd, arithmetic_operation_counter++, equality_to_asm_command));
			}
		}

		// Maybe put in an infinite loop at the end of execution as a safeguard ?

		// Temp output, will change to file output later!
		for (const auto& temp_asm : temp_asm_arr)
		{
			std::cout << temp_asm << '\n';
		}
	}

	static std::string HACK_get_arithmetic_asm(const std::string& cmd, const uint32_t count, const std::unordered_map<std::string, std::string>& eq_to_asm)
	{
		// Technically we don't need to add more M=0 calls after we've moved the stack pointer...
		std::string asm_command;
		if (cmd == "add") asm_command = "M=D+M";
		if (cmd == "sub") asm_command = "M=M-D";

		// Can definitely be optimized with better fallthrough logic!
		if (eq_to_asm.contains(cmd))
		{
			asm_command =
				std::format(
					"D=M-D\n"
					"@SETNEG1_{}\n"
					"{}\n"
					"@SET0_{}\n"
					"0;JMP\n"
					"(SET0_{})\n"
					"@SP\n"
					"A=M-1\n"
					"M=0\n"
					"@END_ARITH_{}\n"
					"0;JMP\n"
					"(SETNEG1_{})\n"
					"@SP\n"
					"A=M-1\n"
					"M=-1\n"
					"(END_ARITH_{})\n", count, eq_to_asm.at(cmd), count, count, count, count, count);
		}

		return "@SP\nM=M-1\nA=M\nD=M\n@SP\nA=M-1\n" + asm_command;
	}

	std::string HACK_get_pop_push_asm(const std::string& destination, int32_t value) const
	{

		if (current_command_type_ == c_push) // expand to support more destinations with proper offset
		{
			if (destination == "constant") return std::format("@{}\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1", value); //push constant n
			if (destination == "static") return std::format("@{}\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1", static_address_space + value); //push static n => push whatever is stored at "static n" address onto the stack (0x100 + n)
		}
		return std::format("@SP\nM=M-1\n@SP\nA=M\nD=M\n@{}\nM=D", static_address_space + value); // pop static n - previous: @SP\nM=M-1\n@SP\nA=M\nD=M\n***M=0***\n@{}\nM=D" (calling M=0 is unnecessary)
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
		if (command == "eq" || command == "lt" || command == "gt" || command == "and" || command == "or" || 
			command == "not" || command == "add" || command == "sub" || command == "neg")
			return c_arithmetic;

		if (check_operation_modify_command(command, "push ")) return c_push;
		if (check_operation_modify_command(command, "pop ")) return c_pop;

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
