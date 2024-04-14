# CMakeList.txt : CMake project for HackAssembler, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (HackAssembler "HackAssembler.cpp" "HackAssembler.h" "Parser.cpp" "Parser.h" "Symbol.cpp" "VMTranslator.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET HackAssembler PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
