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
	{
		this->instruction = "0" + std::bitset<A_INSTRUCTION_SIZE>(registerValue).to_string();

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

	std::string dest;
	std::string comp;
	std::string jump;

	CInstruction(std::string command)
	{
		this->dest = getInstructionDestination(command);
		this->comp = getInstructionComp(command);
		this->jump = getInstructionJump(command);

		std::cout << getBinaryRepr() << std::endl;
	}

	std::string getBinaryRepr()
	{
		return "111" + comp + dest + jump;
	}
};

int main()
{
	// TODO: initial parse to find any symbols to populate within symboltable + remove from commands below
	// Also we should read this from a file
	std::vector<std::string> commands = {
		"@15",
		"M=A",
		"D=M",
		"@0",
		"D;JGE"
	};

	// We don't even need these
	std::vector<CInstruction> CInstructions;
	std::vector<AInstruction> AInstructions;

	for (const auto& command : commands)
	{
		//std::cout << command << std::endl;
		
		switch (getInstructionType(command))
		{
			case(InstructionType::A_INSTRUCTION):
				//std::cout << "A instruction" << std::endl;
				AInstructions.emplace_back(AInstruction(std::stoi(getInstructionSymbol(command))));
				break;
			case(InstructionType::C_INSTRUCTION):
				//std::cout << "C instruction" << std::endl;
				CInstructions.emplace_back(CInstruction(command));
				break;
			default:
				throw std::runtime_error("Invalid command detected"); //TODO: throw in a proper for loop for line numbering
				break;
		}
	}


	/*
	// Don't modify original instruction, copy
	std::string LInstruction = "(xxx)";
	std::cout << "Instruction: " << getInstructionSymbol(LInstruction) << std::endl;
	std::cout << "Original instruction: " << LInstruction << std::endl;

	std::string CInstruction = "D=D+1;JLE"; //C-instruction
	std::cout << "Instruction Dest: " << getInstructionDestination(CInstruction) << std::endl;
	std::cout << "Instruction Comp: " << getInstructionComp(CInstruction) << std::endl;
	std::cout << "Instruction Jump: " << getInstructionJump(CInstruction) << std::endl;
	std::cout << "Original instruction: " << CInstruction << std::endl;
	*/

	return 0;
}
