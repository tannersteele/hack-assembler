#include <string>
#include <unordered_map>
#include "Parser.h"

/*
	TODO: Make this a state-encapsulated class
*/
std::unordered_map<std::string, int> symbolTable
{
	{ "R0",     0 },
	{ "R1",     1 },
	{ "R2",     2 },
	{ "R3",     3 },
	{ "R4",     4 },
	{ "R5",     5 },
	{ "R6",     6 },
	{ "R7",     7 },
	{ "R8",     8 },
	{ "R9",     9 },
	{ "R10",    10 },
	{ "R11",    11 },
	{ "R12",    12 },
	{ "R13",    13 },
	{ "R14",    14 },
	{ "R15",    15 },
	{ "SP",     0  },
	{ "LCL",    1  },
	{ "ARG",    2  },
	{ "THIS",   3  },
	{ "THAT",   4  },
	{ "SCREEN", 16384 },
	{ "KBD",    24576 }
};

std::unordered_map<std::string, int>& getSymbolTable()
{
	return symbolTable;
}

InstructionType getInstructionType(const std::string& instruction)
{
	//HACK: formalize with some switch->case logic & improve parse schema
	if (containsSubstring(instruction, "@"))
		return A_INSTRUCTION;
	if (containsSubstring(instruction, "("))
		return L_INSTRUCTION;

	// Just default as a C-instruction for now
	return C_INSTRUCTION;
}

// Only for A_INSTRUCTION
std::string getInstructionSymbol(std::string instruction) // copied for now since it's being modified
{
	std::erase(instruction, '@');
	return instruction;
}

std::unordered_map<std::string, std::string> destSymbolInstructionMapping = 
{
	{"null", "000"}, // Value not stored
	{"M",    "001"}, // RAM[A]
	{"D",    "010"}, // D register
	{"DM",   "011"}, // D register and RAM[A]
	{"MD",   "011"}, // D register and RAM[A] - dupe
	{"A",    "100"}, // A register
	{"AM",   "101"}, // A register and RAM[A]
	{"MA",   "101"}, // A register and RAM[A] - dupe
	{"AD",   "110"}, // A register and D register
	{"DA",   "110"}, // A register and D register
	{"ADM",  "111"}  // A register, D register, and RAM[A] (only keeping this permutation of it..)
};

std::string getInstructionDestination(const std::string& instruction)
{
	const size_t pos = instruction.find('=');
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
	// would be nice to parse these out before getting here
	size_t posEqual = instruction.find('=');
	if (posEqual != std::string::npos)
		instruction = instruction.substr(posEqual + 1);

	size_t posSemi = instruction.find(';');
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

std::string getInstructionJump(const std::string& instruction)
{
	const size_t pos = instruction.find(';');
	return pos != std::string::npos ?
		jumpSymbolInstructionMapping[instruction.substr(pos + 1)] 
		: jumpSymbolInstructionMapping["null"];
}

bool containsSubstring(const std::string& str, const std::string& substr)
{
	return str.find(substr) != std::string::npos;
}