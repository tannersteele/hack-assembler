#include <unordered_map>
#include <string>

std::unordered_map<std::string, std::string> jumpSymbolInstructionMapping;
std::unordered_map<std::string, std::string> destSymbolInstructionMapping;

// Easier to keep these separate for state of 'a' bit within C-instruction
// Must check which table the instruction is in, add append 0 or 1 comp bit to the beginning based on that, keeping them separate makes this easier
std::unordered_map<std::string, std::string> compA0SymbolInstructionMapping;
std::unordered_map<std::string, std::string> compA1SymbolInstructionMapping;

/*
	Module should have functions which can produce the following:
		dest("DM")  =>     011
		comp("A+1") => 0110111
		comp("M+1") => 1110111
		jump("JNE") =>     101
*/

//TODO: force uppercase everything we're reading in
void populateInstructionMappingTables()
{
	// Could std::bitset<3> a number we're incrementing, but it's cleaner from a reference point to do it this way
	jumpSymbolInstructionMapping = {
		{"null", "000"}, // No jump
		{"JGT",  "001"}, // if comp > 0; jump
		{"JEQ",  "010"}, // if comp = 0; jump
		{"JGE",  "011"}, // if comp >= 0; jump
		{"JLT",  "100"}, // if comp < 0; jump
		{"JNE",  "101"}, // if comp != 0; jump
		{"JLE",  "110"}, // if comp <= 0; jump
		{"JMP",  "111"}  // unconditional jump
	};

	// Effect: store 'comp' from ALU output in...
	destSymbolInstructionMapping = {
		{"null", "000"}, // Value not stored
		{"M",    "001"}, // RAM[A]
		{"D",    "010"}, // D register
		{"DM",   "011"}, // D register and RAM[A]
		{"A",    "100"}, // A register
		{"AM",   "101"}, // A register and RAM[A]
		{"AD",   "110"}, // A register and D register
		{"ADM",  "111"}  // A register, D register, and RAM[A]
	};

	// Must use Hack ASM version of consts (using A-register trick) for any number != 0, 1, or -1
	compA0SymbolInstructionMapping = {
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
	compA1SymbolInstructionMapping = {
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
}