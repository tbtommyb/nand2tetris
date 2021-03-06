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
                       { "push", CommandType::C_PUSH },
                       { "pop", CommandType::C_POP },
                       { "label", CommandType::C_LABEL },
                       { "goto", CommandType::C_GOTO },
                       { "if-goto", CommandType::C_IF },
                       { "function", CommandType::C_FUNCTION },
                       { "return", CommandType::C_RETURN },
                       { "call", CommandType::C_CALL }
};

Parser::Parser(std::istream& input) : source(input) { };

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
    if (currentCommand.empty()) {
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
    case CommandType::C_RETURN:
        command.arg1 = elems[0];
        break;
    case CommandType::C_PUSH:
    case CommandType::C_POP:
    case CommandType::C_CALL:
    case CommandType::C_FUNCTION:
        command.arg1 = elems[1];
        command.arg2 = std::stoi(elems[2]);
        break;
    case CommandType::C_LABEL:
    case CommandType::C_GOTO:
    case CommandType::C_IF:
        command.arg1 = elems[1];
        break;
    }

    return command;
};

// TODO - not handling comments and empty lines correctly
std::string Parser::sanitise(std::string s)
{
    auto commentPos = s.find("//");
    if (commentPos != std::string::npos) {
        s.erase(commentPos);
    }
    return s;
};
