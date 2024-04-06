// HackAssembler.cpp : Defines the entry point for the application.
//

#include "HackAssembler.h"
#include "Parser.h"
#include <vector>
#include <bitset>

const int A_INSTRUCTION_SIZE = 15; // A instructions must be 15 bits wide
const int USER_VAR_SPACE_BEGINNING = 16;

const std::string C_INSTRUCTION_PREFIX = "111";
const std::string A_INSTRUCTION_PREFIX = "0";

int VARS_DECLARED = 0;


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
		std::cout << getBinaryRepr() << std::endl;
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

		std::cout << getBinaryRepr() << std::endl;
	}

	std::string getBinaryRepr()
	{
		return C_INSTRUCTION_PREFIX + this->instruction;
	}
};

bool isNumeric(const std::string& str)
{
	return !str.empty() &&
		std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
}

int main()
{
	// TODO: initial parse to find any symbols to populate within symboltable + remove from commands below
	// Also we should read this from a file

	std::vector<std::string> commands = {
		"@SCREEN", //symbol test
		"M=-1",
		"@TESTER",
		"@TESTER",
		"D=M",
		"@0",
		"D;JLE"
	};

	std::unordered_map < std::string,int>& symTable = getSymbolTable();

	uint16_t currLine = -1;
	for (const auto& command : commands)
	{
		currLine++;
		switch (getInstructionType(command)) // if we store this from the first round of processing, we don't need to re-fetch it's instruction type
		{
			case (InstructionType::A_INSTRUCTION):
			{
				std::string instruction = getInstructionSymbol(command);
				if (symTable.contains(instruction))
				{
					AInstruction aInstruction{ symTable[instruction] };
					continue;
				}

				if (isNumeric(instruction))
				{
					AInstruction aInstruction{ std::stoi(instruction) };
					continue;
				}

				int symbolicReferenceRegister = USER_VAR_SPACE_BEGINNING + (VARS_DECLARED++);

				symTable[instruction] = symbolicReferenceRegister;
				AInstruction aInstruction{ symbolicReferenceRegister };
				break;
			}
			case (InstructionType::C_INSTRUCTION):
			{
				CInstruction cInstruction{ command };
				break;
			}
			default:
			{
				throw std::runtime_error("Invalid command detected"); //TODO: throw in a proper for loop for line numbering
				break;
			}
		}
	}

	return 0;
}
