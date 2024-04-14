#include <string>
#include <vector>
/*
	Test stuff - WIP
*/
enum CommandType
{
	NONE			= 0,
	C_ARITHMETIC		= 1 << 0,
	C_PUSH			= 1 << 1,
	C_POP			= 1 << 2,
	C_LABEL			= 1 << 3,
	C_GOTO			= 1 << 4,
	C_IF			= 1 << 5,
	C_FUNCTION		= 1 << 6,
	C_RETURN		= 1 << 7,
	C_CALL			= 1 << 8
};

class Parser
{
private:
	CommandType current_command_type;
	std::string current_command;

public:
	Parser()
	{
		// Some test commands..
		std::vector<std::string> commands = 
		{
			"push constant 111", "push constant 333", "push constant 888", "pop static 8"
		};
	}

	CommandType get_command_type()
	{
	}

	std::string get_first_arg()
	{
		if (current_command_type == CommandType::C_RETURN)
			return "";

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
	Parser p;
	return 0;
}
