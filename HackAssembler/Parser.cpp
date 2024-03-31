#include <string>
#include <iostream>
#include "Parser.h"
#include <algorithm>

/*
	TODO: Make this a state-encapsulated class
*/

InstructionType getInstructionType(std::string instruction)
{
	//if (containsSubstring(instruction, "@"))
	//{
	//	return InstructionType::A_INSTRUCTION;
	//}

	return containsSubstring(instruction, "@") ? InstructionType::A_INSTRUCTION : InstructionType::NONE;
}

// Only for A_INSTRUCTION or L_INSTRUCTION
std::string getInstructionSymbol(std::string instruction)
{
	InstructionType type = getInstructionType(instruction);

	// Need a comparison against symbol table
	if (type == InstructionType::A_INSTRUCTION)
	{
		instruction.erase(std::remove(instruction.begin(), instruction.end(), '@'), instruction.end());
		return instruction;
	}
	return instruction;
}

std::string getInstructionDestination(std::string instruction)
{
	//STUB
	return instruction;
}


bool containsSubstring(const std::string& str, const std::string& substr)
{
	return str.find(substr) != std::string::npos;
}