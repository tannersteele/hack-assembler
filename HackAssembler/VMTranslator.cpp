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
		const std::vector<std::string> commands = {};


		// Shove all these in a utility file somewhere
		const std::unordered_map<std::string, std::string> comparison_to_asm_command =
		{
			{"eq", "D;JEQ"},
			{"gt", "D;JGT"},
			{"lt", "D;JLT"}
		};

		const std::unordered_map<std::string, uint32_t> destination_to_mem_address =
		{
			{"local",    0x001},
			{"argument", 0x002},
			{"this",     0x003},
			{"that",     0x004}
		};

		const std::unordered_map<std::string, std::string> arithmetic_op_to_asm =
		{
			{"add", "M=M+D"},
			{"sub", "M=M-D"},
			{"and", "M=M&D"},
			{"neg", "M=-M"},
			{"not", "M=!M"},
			{"or",  "M=M|D"}
		};
		//END

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

				temp_asm_arr.emplace_back(HACK_get_pop_push_asm(first_arg, std::stoi(second_arg), destination_to_mem_address));
			}
			else
			{
				temp_asm_arr.emplace_back(HACK_get_arithmetic_asm(cmd, arithmetic_operation_counter++, comparison_to_asm_command, arithmetic_op_to_asm));
			}
		}

		// Maybe put in an infinite loop at the end of execution as a safeguard ?

		// Temp output, will change to file output later!
		for (const auto& temp_asm : temp_asm_arr)
			std::cout << temp_asm << '\n';
	}

	constexpr static std::string fetch_push_indirection_command(uint32_t mem_address, int32_t value)
	{
		return std::format("@{}\nD=M\n@{}\nD=D+A\nA=D\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1", mem_address, value);
	}

	constexpr static std::string fetch_pop_indirection_command(uint32_t mem_address, int32_t value)
	{
		return std::format("@{}\nD=A\n@{}\nD=D+M\n@R13\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D", mem_address, value);
	}

	static std::string HACK_get_arithmetic_asm(
		const std::string& cmd, 
		const uint32_t count, 
		const std::unordered_map<std::string, std::string>& comp_to_asm,
		const std::unordered_map<std::string, std::string>& arithmetic_op_to_asm)
	{
		// Technically we don't need to add more M=0 calls after we've moved the stack pointer...
		std::string asm_command = arithmetic_op_to_asm.at(cmd);

		// Can definitely be optimized with better fallthrough logic!
		if (comp_to_asm.contains(cmd))
		{
			asm_command =
				std::format(
					"D=M-D\n"
					"@SET_NEG1_{}\n"
					"{}\n"
					"@SET0_{}\n"
					"0;JMP\n"
					"(SET0_{})\n"
					"@SP\n"
					"A=M-1\n"
					"M=0\n"
					"@END_ARITHMETIC_{}\n"
					"0;JMP\n"
					"(SET_NEG1_{})\n"
					"@SP\n"
					"A=M-1\n"
					"M=-1\n"
					"(END_ARITHMETIC_{})", count, comp_to_asm.at(cmd), count, count, count, count, count);
		}

		if (cmd == "not" || cmd == "neg") //unary operation, only move the stack pointer back once
			return std::format("@SP\nM=M-1\nA=M\nD=M\n{}\n@SP\nM=M+1", asm_command);
			
		return "@SP\nM=M-1\nA=M\nD=M\n@SP\nA=M-1\n" + asm_command;
	}

	std::string HACK_get_pop_push_asm(const std::string& destination, int32_t value, const std::unordered_map<std::string, uint32_t>& dest_to_mem_addr) const
	{
		if (current_command_type_ == c_push) // expand to support more destinations with proper offset
		{
			// Remove all this duped crap
			if (destination == "constant") return std::format("@{}\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1", value); //push constant n
			if (destination == "static")   return std::format("@{}\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1", static_address_space + value); //push static n => push whatever is stored at "static n" address onto the stack (0x100 + n)

			if (destination == "local" || destination == "argument" || destination == "this" || destination == "that") // use some enum/state var to repr these instead
				return fetch_push_indirection_command(dest_to_mem_addr.at(destination), value);

			if (destination == "pointer")  return std::format("@{}\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1", 0x003 + value);
			if (destination == "temp")     return std::format("@{}\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1", 0x005 + value);
		}

		// pop commands
		if (destination == "local" || destination == "argument" || destination == "this" || destination == "that")
			return fetch_pop_indirection_command(dest_to_mem_addr.at(destination), value);

		if (destination == "pointer")  return std::format("@SP\nM=M-1\n@SP\nA=M\nD=M\n@{}\nM=D", 0x003 + value);
		if (destination == "temp")     return std::format("@SP\nM=M-1\n@SP\nA=M\nD=M\n@{}\nM=D", 0x005 + value);

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
