// HackAssembler.cpp : Defines the entry point for the application.
//

#include "HackAssembler.h"
#include "Parser.h"
#include <vector>
#include <bitset>
#include <fstream>
#include <algorithm>
#include <chrono>

const int32_t A_INSTRUCTION_SIZE = 15; // A instructions must be 15 bits wide
const int32_t USER_VAR_SPACE_BEGINNING = 16;
const int32_t ARGC_EXPECTED_COUNT = 3;
const int32_t COMMAND_BUFFER_INIT_SIZE = 30000;

const std::string C_INSTRUCTION_PREFIX = "111";
const std::string A_INSTRUCTION_PREFIX = "0";
const std::string COMMENT_TOKEN = "//";


struct Instruction
{
	virtual std::string get_binary_repr() const = 0;
};

struct AInstruction : Instruction
{
	std::string instruction;

	explicit AInstruction(int register_value)
		: instruction(::A_INSTRUCTION_PREFIX + std::bitset<::A_INSTRUCTION_SIZE>(register_value).to_string()) // Some protection for total size would be nice here
	{
	}

	inline std::string get_binary_repr() const override
	{
		return instruction;
	}
};

struct CInstruction : Instruction
{
	std::string instruction;

	explicit CInstruction(std::string command)
	{
		instruction += getInstructionComp(command);
		instruction += getInstructionDestination(command);
		instruction += getInstructionJump(command);

		instruction = C_INSTRUCTION_PREFIX + instruction;
	}

	std::string get_binary_repr() const override
	{
		return instruction;
	}
};

//Move utility func
inline void strip_spaces(std::string& str)
{
	str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
}


// TODO: Refactor + optimize + get rid of hardcoded stuff, QoL stuff
//       Auto capitalize stuff
//       Support same line commenting (split everything past // on a line)
int _main(int argc, char* argv[])
{
	if (argc != ARGC_EXPECTED_COUNT)
	{
		std::cerr << "Usage: HackAssembler.exe <input file> <output file>" << std::endl;
		return 1;
	}

	std::ifstream file(argv[1]); //argv[1] = input file

	std::vector<std::string> commands;
	commands.reserve(::COMMAND_BUFFER_INIT_SIZE); // heap allocated..we could speed this up(cache locality) - but SBO might make this negligible

	std::unordered_map<std::string,int> symTable = getSymbolTable();

	uint16_t currLine = 0;

	auto start = std::chrono::high_resolution_clock::now();
	std::string line;
	while (std::getline(file, line))
	{
		if (line == "")
			continue;

		strip_spaces(line);

		if (line.compare(0, COMMENT_TOKEN.size(), COMMENT_TOKEN) == 0)
			continue;

		//Ensure anything in a comment is factored out BY THIS POINT

		// Move to func
		if (line.find("(") != std::string::npos)
		{
			line.erase(std::remove_if(line.begin(), line.end(),
				[](char c) { return c == '(' || c == ')'; }), line.end()
			);

			if (symTable.contains(line))
				continue;

			symTable[line] = currLine;
			continue;
		}

		currLine++;
		commands.emplace_back(line);
	}

	int32_t local_vars_declared = 0;

	std::ofstream output_hack_file(argv[2]);
	for (const auto& command : commands)
	{
		Instruction *currInstruction;
		switch (getInstructionType(command))
		{
			case (InstructionType::A_INSTRUCTION):
			{
				int32_t registerValue = 0;
				std::string symbol = getInstructionSymbol(command);

				if (symTable.contains(symbol))
				{
					registerValue = symTable[symbol];
				}
				else if (std::all_of(symbol.begin(), symbol.end(), ::isdigit))
				{
					registerValue = std::stoi(symbol);
				}
				else
				{
					int symbolicReferenceRegister = ::USER_VAR_SPACE_BEGINNING + (local_vars_declared++);

					symTable[symbol] = symbolicReferenceRegister;
					registerValue = symbolicReferenceRegister;
				}

				AInstruction aInstruction{ registerValue };
				currInstruction = &aInstruction;
				break;
			}
			case (InstructionType::C_INSTRUCTION):
			{
				CInstruction cInstruction{ command };
				currInstruction = &cInstruction;
				break;
			}
			default:
			{
				std::cerr << "Invalid InstructionType detected during parsing!" << std::endl;
				return -1;
			}
		}
		output_hack_file << currInstruction->get_binary_repr() << std::endl;
	}
	output_hack_file.close(); //wrap in some class and add to dtor
	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms." << std::endl;

	return 0;
}