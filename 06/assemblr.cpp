#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "parser.hpp"
#include "code.hpp"

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

    Parser parser{prog};

    while (parser.hasMoreCommands()) {
        parser.advance();
        Instruction instruction;

        try {
            instruction = parser.parse();
        } catch (const InvalidCommand& e) {
            std::cerr << "Invalid command: " << e.what() << std::endl;
            exit(1);
        }

        auto code = Code{instruction};
        out << code.string() << std::endl;
    }

    out.close();

    return 0;
};
