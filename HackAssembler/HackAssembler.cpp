// HackAssembler.cpp : Defines the entry point for the application.
//

#include "HackAssembler.h"
#include "Parser.h"

int main()
{
	// Don't modify original instruction, copy
	std::string instruction = "(xxx)";
	std::cout << "Instruction: " << getInstructionSymbol(instruction) << std::endl;
	std::cout << "Original instruction: " << instruction << std::endl;

	return 0;
}
