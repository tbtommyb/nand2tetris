#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "parser.hpp"
#include "code.hpp"
#include "symbol_table.hpp"

std::string getFilename(std::string input)
{
    std::istringstream iss{input};
    if (std::getline(iss, input, '.')) {
        return input;
    }
    return "";
};

int  main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "USAGE: assemblr input.asm" << std::endl;
        exit(1);
    }
    auto input = argv[1];
    std::ifstream prog{input};

    auto filename = getFilename(input);
    std::ofstream out{filename + ".hack"};

    SymbolTable symbols{};
    Parser symbolParser{prog};
    unsigned int instructionAddress = 0x0000;

    while (symbolParser.hasMoreCommands()) {
        symbolParser.advance();
        Instruction instruction;

        try {
            instruction = symbolParser.parse();
        } catch (const InvalidCommand& e) {
            std::cerr << "Invalid command: " << e.what() << std::endl;
            exit(1);
        }

        switch (instruction.type) {
        case C_COMMAND:
        case A_COMMAND:
            instructionAddress++;
            break;
        case L_COMMAND:
            symbols.addEntry(instruction.symbol, instructionAddress);
            break;
        }
    }

    std::ifstream prog2{input};
    Parser parser{prog2};

    while (parser.hasMoreCommands()) {
        parser.advance();
        Instruction instruction;

        try {
            instruction = parser.parse();
        } catch (const InvalidCommand& e) {
            std::cerr << "Invalid command: " << e.what() << std::endl;
            exit(1);
        }

        auto code = Code{instruction, symbols};
        if (instruction.type != L_COMMAND) {
            out << code.string() << std::endl;
        }
    }

    out.close();

    return 0;
};
