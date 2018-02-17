#include <fstream>
#include <iostream>
#include <sstream>
#include "parser.hpp"

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

    while (parser.hasMoreCommands()) {
        parser.advance();

        Command command = parser.parse();
        std::cout << command.type << std::endl;
        std::cout << command.arg1 << std::endl;
        std::cout << command.arg2 << std::endl;
    }

    input.close();
    out.close();

    return 0;
};
