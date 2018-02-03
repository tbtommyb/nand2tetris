#include <regex>
#include <iostream>
#include "parser.hpp"

Parser::Parser(std::ifstream& input) : stream(std::move(input)) { };

bool Parser::hasMoreCommands() noexcept
{
    auto c = stream.peek();
    if (c == EOF) {
        std::cout << "false" << std::endl;
        return false;
    }
    std::cout << "true" << std::endl;
    return true;
};

void Parser::advance()
{
    std::getline(stream, currentLine);
    std::cout << currentLine << std::endl;
};

std::experimental::optional<CommandType> const Parser::commandType()
{
    std::regex A_command{"^@(\\d*)"};
    std::regex C_command{"(\\d*)=(\\d*);(\\d*)"};
    std::regex C_command_no_dest{"(\\d*);(\\d*)"};
    std::regex C_command_no_jump{"(\\d*)=(\\d*)"};
    std::smatch match;

    if (std::regex_match(currentLine, match, A_command)) {
        if (match.size() == 2) {
            A_value = match[1].str();

            return CommandType::A_COMMAND;
        }
    }

    if (std::regex_match(currentLine, match, C_command)) {
        if (match.size() == 4) {
            C_dest = match[1].str();
            C_comp = match[2].str();
            C_jump = match[3].str();

            return CommandType::C_COMMAND;
        }
    }

    if (std::regex_match(currentLine, match, C_command_no_dest)) {
        if (match.size() == 3) {
            C_comp = match[1].str();
            C_jump = match[2].str();

            return CommandType::C_COMMAND;
        }
    }

    if (std::regex_match(currentLine, match, C_command)) {
        if (match.size() == 3) {
            C_dest = match[1].str();
            C_comp = match[2].str();

            return CommandType::C_COMMAND;
        }
    }

    return {};
};

std::string const& Parser::symbol()
{
    return A_value;
};

std::string const& Parser::dest()
{
    return C_dest;
};

std::string const& Parser::comp()
{
    return C_comp;
};

std::string const& Parser::jump()
{
    return C_jump;
};

std::string sanitise(std::string input)
{
    std::erase(std::remove_if(input.begin(), input.end(), std::isspace), input.end());
};
