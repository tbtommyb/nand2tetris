#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "parser.hpp"

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

        // TODO improve use of Optional
        auto command = parser.commandType();
        if (!command) { continue; }

        if (*command == C_COMMAND) {
            auto dest = parser.dest();
            auto comp = parser.comp();
            auto jump = parser.jump();
        }
        if ((*command == A_COMMAND) || (*command == L_COMMAND)) {
            auto symbol = parser.symbol();
        }
    }

    out.close();

    return 0;
};
