#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>
#include "parser.hpp"

typedef std::map<std::string, CommandType> CommandMap;

CommandMap commands = {
                       { "add", CommandType::C_ARITHMETIC },
                       { "sub", CommandType::C_ARITHMETIC },
                       { "neg", CommandType::C_ARITHMETIC },
                       { "eq", CommandType::C_ARITHMETIC },
                       { "gt", CommandType::C_ARITHMETIC },
                       { "lt", CommandType::C_ARITHMETIC },
                       { "and", CommandType::C_ARITHMETIC },
                       { "or", CommandType::C_ARITHMETIC },
                       { "not", CommandType::C_ARITHMETIC },
                       { "push", CommandType::C_PUSH }
};

Parser::Parser(std::ifstream& input) : source(input) { };

bool Parser::hasMoreCommands() noexcept
{
    return source.peek() != EOF;
};

void Parser::advance()
{
    std::string input;
    std::getline(source, input);
    currentCommand = sanitise(input);

    // Skip empty lines
    if (currentCommand.length() == 0) {
        advance();
    }
};

Command Parser::parse()
{
    std::stringstream ss{currentCommand};
    std::istream_iterator<std::string> begin{ss};
    std::istream_iterator<std::string> end;
    std::vector<std::string> elems(begin, end);

    auto commandType = commands[elems[0]];
    Command command{ .type = commandType };

    switch (commandType) {
    case CommandType::C_ARITHMETIC:
        command.arg1 = elems[0];
        break;
    case CommandType::C_PUSH:
        command.arg1 = elems[1];
        command.arg2 = std::stoi(elems[2]);
        break;
    }

    return command;
};

std::string Parser::sanitise(std::string s)
{
    auto commentPos = s.find("//");
    if (commentPos != std::string::npos) {
        s.erase(commentPos);
    }
    return s;
};
