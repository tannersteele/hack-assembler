// HackAssembler.cpp : Defines the entry point for the application.
//

#include "HackAssembler.h"
#include "Parser.h"
#include <vector>
#include <bitset>
#include <fstream>

const int A_INSTRUCTION_SIZE = 15; // A instructions must be 15 bits wide
const int USER_VAR_SPACE_BEGINNING = 16;

const int32_t ARGC_EXPECTED_COUNT = 3;
const int32_t COMMAND_BUFFER_INIT_SIZE = 30000;

const std::string C_INSTRUCTION_PREFIX = "111";
const std::string A_INSTRUCTION_PREFIX = "0";

int32_t LOCAL_VARS_DECLARED = 0;


struct Instruction
{
	virtual std::string getBinaryRepr() = 0;
};

struct AInstruction : Instruction
{
	std::string instruction;

	AInstruction(int registerValue)
		: instruction(A_INSTRUCTION_PREFIX + std::bitset<A_INSTRUCTION_SIZE>(registerValue).to_string()) // Some protection for total size would be nice here
	{
	}

	std::string getBinaryRepr()
	{
		return instruction;
	}
};

struct CInstruction : Instruction
{
	std::string instruction;

	CInstruction(std::string command)
	{
		instruction += getInstructionComp(command);
		instruction += getInstructionDestination(command);
		instruction += getInstructionJump(command);
	}

	std::string getBinaryRepr()
	{
		return C_INSTRUCTION_PREFIX + instruction;
	}
};

//Move utility func
bool isNumeric(const std::string& str)
{
	return !str.empty() &&
		std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
}

//Move utility func
void stripSpaces(std::string& s)
{
	s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
}

// TODO: Refactor + optimize + get rid of hardcoded stuff, QoL stuff
//       Auto capitalize stuff
int main(int argc, char* argv[])
{
	if (argc != ARGC_EXPECTED_COUNT)
	{
		std::cerr << "Usage: HackAssembler.exe <input file> <output file>" << std::endl;
		return 1;
	}

	std::ifstream file(argv[1]); //argv[1] = input file

	std::vector<std::string> commandBuffer;
	commandBuffer.reserve(COMMAND_BUFFER_INIT_SIZE); //heap allocated.. we could speed this up (cache locality) - but SBO might make this negligible

	std::string line;
	std::string commentToken = "//";
	while (std::getline(file, line))
	{
		if (line == "") //ignore whitespace lines
			continue;

		stripSpaces(line);
		if (line.compare(0, commentToken.size(), commentToken) == 0) //ignore comments
			continue;

		commandBuffer.emplace_back(line);
	}

	// The actual list of commands
	std::vector<std::string> commands;
	std::unordered_map <std::string, int>& symTable = getSymbolTable();

	uint16_t currLine = 0;
	for (const auto& command : commandBuffer)
	{
		if (getInstructionType(command) == InstructionType::L_INSTRUCTION) // they must be unique
		{
			std::string symbol = getInstructionSymbol(command);
			if (symTable.contains(symbol))
				continue;

			symTable[symbol] = currLine;
			continue;
		}

		currLine++;
		commands.emplace_back(command);
	}
	commandBuffer.clear();

	std::vector<std::string> hackInstructionBuffer;
	hackInstructionBuffer.reserve(COMMAND_BUFFER_INIT_SIZE);
	for (const auto& command : commands)
	{
		switch (getInstructionType(command)) // if we store this from the first round of processing, we don't need to re-fetch it's instruction type
		{
			case (InstructionType::A_INSTRUCTION):
			{
				int registerValue;
				std::string symbol = getInstructionSymbol(command);

				if (symTable.contains(symbol))
				{
					registerValue = symTable[symbol];
				}
				else if (isNumeric(symbol))
				{
					registerValue = std::stoi(symbol);
				}
				else
				{
					int symbolicReferenceRegister = USER_VAR_SPACE_BEGINNING + (LOCAL_VARS_DECLARED++);

					symTable[symbol] = symbolicReferenceRegister;
					registerValue = symbolicReferenceRegister;
				}

				AInstruction aInstruction{ registerValue };
				hackInstructionBuffer.emplace_back(aInstruction.getBinaryRepr());
				break;
			}
			case (InstructionType::C_INSTRUCTION):
			{
				// Could write these instructions to a generic Instruction array, then when we want to export everything is already in order (polymorphism!)
				CInstruction cInstruction{ command };
				hackInstructionBuffer.emplace_back(cInstruction.getBinaryRepr());
				break;
			}
			default:
			{
				std::cerr << "Invalid InstructionType detected during parsing!" << std::endl;
				break;
			}
		}
	}

	std::ofstream outputHackFile(argv[2]); //argv[2] = output file
	for (const auto& instruction : hackInstructionBuffer)
	{
		outputHackFile << instruction << std::endl;
	}
	outputHackFile.close();

	return 0;
}