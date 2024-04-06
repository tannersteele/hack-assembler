#pragma once

#include <string>

enum InstructionType
{
    A_INSTRUCTION, // for @xxx where xxx is decimal number or symbol
    C_INSTRUCTION, // for dest=comp;jump
    L_INSTRUCTION, // for (xxx) where xxx is a symbol
    NONE
};

InstructionType getInstructionType(const std::string& instruction);

int getInstructionSymbol(std::string instruction);

std::string getInstructionDestination(std::string instruction);

std::string getInstructionComp(std::string instruction);

std::string getInstructionJump(std::string instruction);

bool containsSubstring(const std::string& str, const std::string& substr);