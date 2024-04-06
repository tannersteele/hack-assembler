#include <string>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <unordered_map>

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

std::unordered_map<std::string, std::string> destSymbolInstructionMapping = 
{
	{"null", "000"}, // Value not stored
	{"M",    "001"}, // RAM[A]
	{"D",    "010"}, // D register
	{"DM",   "011"}, // D register and RAM[A]
	{"A",    "100"}, // A register
	{"AM",   "101"}, // A register and RAM[A]
	{"AD",   "110"}, // A register and D register
	{"ADM",  "111"}  // A register, D register, and RAM[A]
};

std::string getInstructionDestination(std::string instruction)
{
	size_t pos = instruction.find('=');
	return pos != std::string::npos ? 
		destSymbolInstructionMapping[instruction.substr(0, pos)] : 
		destSymbolInstructionMapping["null"];
}

// Must use Hack ASM version of consts (using A-register trick) for any number != 0, 1, or -1
std::unordered_map<std::string, std::string> compA0SymbolInstructionMapping = {
	{"0",   "101010"},
	{"1",   "111111"},
	{"-1",  "111010"},
	{"D",   "001100"},
	{"A",   "110000"},
	{"!D",  "001101"},
	{"!A",  "110001"},
	{"-D",  "001111"},
	{"-A",  "110011"},
	{"D+1", "011111"},
	{"A+1", "110111"},
	{"D-1", "001110"},
	{"A-1", "110010"},
	{"D+A", "000010"},
	{"D-A", "010011"},
	{"A-D", "000111"},
	{"D&A", "000000"},
	{"D|A", "010101"}
};

// Fortunately hack is designed in such a way that M instructions are tied to the `a` bit being 1, this makes it much easier to implement in hardware!
// (can just mux the `a` bit to determine register values we need to operate on) => output from A register + input from 'M'
std::unordered_map<std::string, std::string> compA1SymbolInstructionMapping = {
	{"M",   "110000"},
	{"!M",  "110001"},
	{"-M",  "110001"},
	{"M+1", "110111"},
	{"M-1", "110010"},
	{"D+M", "000010"},
	{"D-M", "010011"},
	{"M-D", "000111"},
	{"D&M", "000000"},
	{"D|M", "010101"}
};

std::string getInstructionComp(std::string instruction)
{
	size_t posEqual = instruction.find('=');
	if (posEqual != std::string::npos)
		instruction = instruction.substr(posEqual + 1);

	size_t posSemi = instruction.find(";");
	if (posSemi != std::string::npos)
		instruction = instruction.substr(0, posSemi);

	return compA0SymbolInstructionMapping.contains(instruction)
		? std::string("0" + compA0SymbolInstructionMapping[instruction])  // a=0
		: std::string("1" + compA1SymbolInstructionMapping[instruction]); // a=1
}

std::unordered_map<std::string, std::string> jumpSymbolInstructionMapping = 
{
		{"null", "000"}, // No jump
		{"JGT",  "001"}, // if comp > 0; jump
		{"JEQ",  "010"}, // if comp = 0; jump
		{"JGE",  "011"}, // if comp >= 0; jump
		{"JLT",  "100"}, // if comp < 0; jump
		{"JNE",  "101"}, // if comp != 0; jump
		{"JLE",  "110"}, // if comp <= 0; jump
		{"JMP",  "111"}  // unconditional jump
};

std::string getInstructionJump(std::string instruction)
{
	size_t pos = instruction.find(';');
	return pos != std::string::npos ?
		jumpSymbolInstructionMapping[instruction.substr(pos + 1)] 
		: jumpSymbolInstructionMapping["null"];
}

bool containsSubstring(const std::string& str, const std::string& substr)
{
	return str.find(substr) != std::string::npos;
}