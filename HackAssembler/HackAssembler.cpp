// HackAssembler.cpp : Defines the entry point for the application.
//

#include "HackAssembler.h"
#include "Parser.h"
#include <vector>
#include <bitset>

const int A_INSTRUCTION_SIZE = 15; // A instructions must be 15 bits wide
const int USER_VAR_SPACE_BEGINNING = 16;

const std::string C_INSTRUCTION_PREFIX = "111";
const std::string A_INSTRUCTION_PREFIX = "0";

int VARS_DECLARED = 0;


struct Instruction
{
	virtual std::string getBinaryRepr() = 0;
};

struct AInstruction : Instruction
{
	std::string instruction;

	AInstruction(int registerValue)
		: instruction(A_INSTRUCTION_PREFIX + std::bitset<A_INSTRUCTION_SIZE>(registerValue).to_string()) // Some protection for total size would be nice here
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
		return C_INSTRUCTION_PREFIX + this->instruction;
	}
};

bool isNumeric(const std::string& str)
{
	return !str.empty() &&
		std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
}

int main()
{
	// TODO: Refactor + optimize
	//       Read in ASM from a file
	//       Ignore whitespace and comments in parse step

	// Will formalize this a bit more in the future, but in the mean time this acts as file input (with tokens that should NOT make it to second phase!)
	std::vector<std::string> commandsReadIn = {
		"(OUTERLOOP)",
		"@SCREEN",
		"D=A",
		"@SCREENPTR",
		"M=D",
		"@PIXELCOLOR",
		"M=0",
		"@KBD",
		"D=M",
		"@SETBLACK",
		"D;JNE",
		"@INNERLOOP",
		"0;JMP",
		"(INNERLOOP)",
		"@PIXELCOLOR",
		"D=M",
		"@SCREENPTR",
		"A=M",
		"M=D",
		"@SCREENPTR",
		"M=M+1",
		"D=M",
		"@24575",
		"D=D-A",
		"@OUTERLOOP",
		"D;JGT",
		"@INNERLOOP",
		"0;JMP",
		"(SETBLACK)",
		"@PIXELCOLOR",
		"M=-1",
		"@INNERLOOP",
		"0;JMP"
	};

	// The actual list of commands
	std::vector<std::string> commands;
	std::unordered_map <std::string, int>& symTable = getSymbolTable();

	uint16_t currLine = 0;
	for (const auto& command : commandsReadIn)
	{
		if (getInstructionType(command) == InstructionType::L_INSTRUCTION) // they must be unique
		{
			std::string symbol = getInstructionSymbol(command);
			if (symTable.contains(symbol))
				continue;

			symTable[symbol] = currLine;
			continue;
		}

		currLine++;
		commands.emplace_back(command);
	}

	for (const auto& command : commands)
	{
		switch (getInstructionType(command)) // if we store this from the first round of processing, we don't need to re-fetch it's instruction type
		{
			case (InstructionType::A_INSTRUCTION):
			{
				std::string symbol = getInstructionSymbol(command);
				if (symTable.contains(symbol))
				{
					AInstruction aInstruction{ symTable[symbol] };
					continue;
				}

				if (isNumeric(symbol))
				{
					AInstruction aInstruction{ std::stoi(symbol) };
					continue;
				}

				int symbolicReferenceRegister = USER_VAR_SPACE_BEGINNING + (VARS_DECLARED++);

				symTable[symbol] = symbolicReferenceRegister;
				AInstruction aInstruction{ symbolicReferenceRegister };
				break;
			}
			case (InstructionType::C_INSTRUCTION):
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
