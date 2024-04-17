// HackAssembler.cpp : Defines the entry point for the application.
//

#include "HackAssembler.h"
#include "Parser.h"
#include <vector>
#include <bitset>
#include <fstream>
#include <algorithm>
#include <chrono>

constexpr int32_t A_INSTRUCTION_SIZE = 15; // A instructions must be 15 bits wide
constexpr int32_t USER_VAR_SPACE_BEGINNING = 16;
constexpr int32_t ARGC_EXPECTED_COUNT = 3;
constexpr int32_t COMMAND_BUFFER_INIT_SIZE = 30000;

const std::string C_INSTRUCTION_PREFIX = "111";
const std::string A_INSTRUCTION_PREFIX = "0";
const std::string COMMENT_TOKEN = "//";


struct Instruction
{
	virtual ~Instruction() = default;
	virtual std::string get_binary_repr() const = 0;
};

struct AInstruction : Instruction
{
	std::string instruction;

	explicit AInstruction(int register_value)
		: instruction(::A_INSTRUCTION_PREFIX + std::bitset<::A_INSTRUCTION_SIZE>(register_value).to_string()) // Some protection for total size would be nice here
	{
	}

	[[nodiscard]] std::string get_binary_repr() const override
	{
		return instruction;
	}
};

struct CInstruction : Instruction
{
	std::string instruction;

	explicit CInstruction(const std::string& command)
	{
		instruction += getInstructionComp(command);
		instruction += getInstructionDestination(command);
		instruction += getInstructionJump(command);

		instruction = C_INSTRUCTION_PREFIX + instruction;
	}

	[[nodiscard]] std::string get_binary_repr() const override
	{
		return instruction;
	}
};


//Move utility func
inline void strip_spaces(std::string& str)
{
	std::erase_if(str, ::isspace);
}

// TODO: Refactor + optimize + get rid of hardcoded stuff, QoL stuff
//       Auto capitalize stuff
//       Support same line commenting (split everything past // on a line)
int _main(int argc, char* argv[])
{
	if (argc != ARGC_EXPECTED_COUNT)
	{
		std::cerr << "Usage: HackAssembler.exe <input file> <output file>" << '\n';
		return 1;
	}

	std::ifstream file(argv[1]); //argv[1] = input file

	std::vector<std::string> commands;
	commands.reserve(::COMMAND_BUFFER_INIT_SIZE); // heap allocated..we could speed this up(cache locality) - but SBO might make this negligible

	std::unordered_map<std::string,int> sym_table = getSymbolTable();

	uint16_t curr_line = 0;

	auto start = std::chrono::high_resolution_clock::now();
	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		strip_spaces(line);

		if (line.compare(0, COMMENT_TOKEN.size(), COMMENT_TOKEN) == 0)
			continue;

		//Ensure anything in a comment is factored out BY THIS POINT

		// Move to func
		if (line.find('(') != std::string::npos)
		{
			std::erase_if(line,[](const char c) { return c == '(' || c == ')'; });

			if (sym_table.contains(line))
				continue;

			sym_table[line] = curr_line;
			continue;
		}

		curr_line++;
		commands.emplace_back(line);
	}

	int32_t local_vars_declared = 0;

	std::ofstream output_hack_file(argv[2]);
	for (const auto& command : commands)
	{
		Instruction *curr_instruction;

		switch (getInstructionType(command))
		{
			case (A_INSTRUCTION):
			{
				int32_t register_value = 0;
				std::string symbol = getInstructionSymbol(command);

				if (sym_table.contains(symbol))
				{
					register_value = sym_table[symbol];
				}
				else if (std::ranges::all_of(symbol, ::isdigit))
				{
					register_value = std::stoi(symbol);
				}
				else
				{
					int symbolicReferenceRegister = ::USER_VAR_SPACE_BEGINNING + (local_vars_declared++);

					sym_table[symbol] = symbolicReferenceRegister;
					register_value = symbolicReferenceRegister;
				}

				AInstruction aInstruction{ register_value };
				curr_instruction = &aInstruction;
				break;
			}
			case (C_INSTRUCTION):
			{
				CInstruction c_instruction{ command };
				curr_instruction = &c_instruction;
				break;
			}
			default:
			{
				std::cerr << "Invalid InstructionType detected during parsing!" << '\n';
				return -1;
			}
		}
		output_hack_file << curr_instruction->get_binary_repr() << '\n';
	}
	output_hack_file.close(); //wrap in some class and add to dtor
	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms." << '\n';

	return 0;
}