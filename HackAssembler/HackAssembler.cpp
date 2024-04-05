// HackAssembler.cpp : Defines the entry point for the application.
//

#include "HackAssembler.h"
#include "Parser.h"

int _main()
{
	// Don't modify original instruction, copy
	std::string LInstruction = "(xxx)";
	std::cout << "Instruction: " << getInstructionSymbol(LInstruction) << std::endl;
	std::cout << "Original instruction: " << LInstruction << std::endl;

	std::string CInstruction = "D=D+1;JLE"; //C-instruction
	std::cout << "Instruction Dest: " << getInstructionDestination(CInstruction) << std::endl;
	std::cout << "Instruction Comp: " << getInstructionComp(CInstruction) << std::endl;
	std::cout << "Instruction Jump: " << getInstructionJump(CInstruction) << std::endl;
	std::cout << "Original instruction: " << CInstruction << std::endl;

	return 0;
}
