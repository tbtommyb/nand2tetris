#include <fstream>
#include <iostream>
#include <sstream>
#include "parser.hpp"
#include "code_writer.hpp"

std::string getName(std::string input)
{
    std::istringstream iss{input};
    if (std::getline(iss, input, '.')) {
        return input;
    }
    return "";
};

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "USAGE: vm input.vm" << std::endl;
        exit(1);
    }

    auto filename = argv[1];
    std::ifstream input{filename};

    auto name = getName(filename);
    std::ofstream out{name + ".asm"};

    Parser parser{input};
    CodeWriter writer{out};

    while (parser.hasMoreCommands()) {
        parser.advance();

        Command command = parser.parse();
        switch(command.type) {
        case CommandType::C_PUSH:
        case CommandType::C_POP:
            writer.writePushPop(command);
            break;
        case CommandType::C_ARITHMETIC:
            writer.writeArithmetic(command);
            break;
        }
    }

    return 0;
};
