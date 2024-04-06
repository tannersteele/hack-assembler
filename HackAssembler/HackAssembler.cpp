// HackAssembler.cpp : Defines the entry point for the application.
//

#include "HackAssembler.h"
#include "Parser.h"
#include <vector>
#include <bitset>

const int A_INSTRUCTION_SIZE = 15; // A instructions must be 15 bits wide

struct Instruction
{
	virtual std::string getBinaryRepr() = 0;
};

struct AInstruction : Instruction
{
	std::string instruction;

	AInstruction(const int registerValue)
		: instruction("0" + std::bitset<A_INSTRUCTION_SIZE>(registerValue).to_string())
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
		return "111" + this->instruction;
	}
};

int main()
{
	// TODO: initial parse to find any symbols to populate within symboltable + remove from commands below
	// Also we should read this from a file

	// Write a black pixel to the screen
	std::vector<std::string> commands = {
		"@SCREEN", //symbol test
		"M=-1",
		"D=M",
		"@0",
		"D;JLE"
	};

	for (const auto& command : commands)
	{
		switch (getInstructionType(command))
		{
			case(InstructionType::A_INSTRUCTION):
			{
				AInstruction aInstruction{ getInstructionSymbol(command) };
				break;
			}
			case(InstructionType::C_INSTRUCTION):
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
