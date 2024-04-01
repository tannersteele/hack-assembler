#include <string>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "Parser.h"

/*
	TODO: Make this a state-encapsulated class
*/

InstructionType getInstructionType(const std::string& instruction)
{
	//HACK: formalize with some switch->case logic & improve parse schema
	if (containsSubstring(instruction, "@"))
		return InstructionType::A_INSTRUCTION;
	else if (containsSubstring(instruction, "("))
		return InstructionType::L_INSTRUCTION;

	return InstructionType::NONE;
}

// Only for A_INSTRUCTION or L_INSTRUCTION
std::string getInstructionSymbol(std::string instruction)
{
	InstructionType type = getInstructionType(instruction);

	//Only process A instructions and L instructions
	assert(type == InstructionType::A_INSTRUCTION || type == InstructionType::L_INSTRUCTION, "Invalid instruction type. `A` or `L` instruction required.");

	// Need a comparison against symbol table
	if (type == InstructionType::A_INSTRUCTION)
	{
		instruction.erase(std::remove(instruction.begin(), instruction.end(), '@'), instruction.end());
		return instruction;
	}
	else if (type == InstructionType::L_INSTRUCTION) // Hacky, formalize with some switch->case logic
	{
		instruction.erase(std::remove_if(instruction.begin(), instruction.end(),
			[](char c) { return c == '(' || c == ')'; }), instruction.end()
		);
	}
	return instruction;
}

std::string getInstructionDestination(const std::string& instruction)
{
	//STUB
	return instruction;
}

bool containsSubstring(const std::string& str, const std::string& substr)
{
	return str.find(substr) != std::string::npos;
}