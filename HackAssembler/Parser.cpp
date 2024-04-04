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

	// Just default as a C-instruction for now
	return InstructionType::C_INSTRUCTION;
}

// Only for A_INSTRUCTION or L_INSTRUCTION
std::string getInstructionSymbol(std::string instruction) // copied for now since it's being modified
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

	instruction.erase(std::remove_if(instruction.begin(), instruction.end(),
		[](char c) { return c == '(' || c == ')'; }), instruction.end()
	);

	return instruction;
}

// Dest is what's left to the equal sign in a full C-instruction
std::string getInstructionDestination(std::string instruction)
{
	InstructionType type = getInstructionType(instruction);

	assert(type == InstructionType::C_INSTRUCTION, "Invalid instruction type. Must be a `C` instruction.");

	size_t pos = instruction.find('=');
	return pos != std::string::npos ? instruction.substr(0, pos) : instruction;
}

// Comp is after the equal sign but before the semicolon in a full C-instruction - could be combined with above logic to compute them all at once
std::string getInstructionComp(std::string instruction)
{
	InstructionType type = getInstructionType(instruction);

	assert(type == InstructionType::C_INSTRUCTION, "Invalid instruction type. Must be a `C` instruction.");

	size_t pos = instruction.find('=');
	return pos != std::string::npos ? instruction.substr(pos + 1, instruction.find(';') - 2) : instruction;
}

// Copy pasta'd from the ones above, will clean it up later!
std::string getInstructionJump(std::string instruction)
{
	InstructionType type = getInstructionType(instruction);

	assert(type == InstructionType::C_INSTRUCTION, "Invalid instruction type. Must be a `C` instruction.");

	size_t pos = instruction.find(';');
	return pos != std::string::npos ? instruction.substr(pos + 1) : instruction;
}

bool containsSubstring(const std::string& str, const std::string& substr)
{
	return str.find(substr) != std::string::npos;
}