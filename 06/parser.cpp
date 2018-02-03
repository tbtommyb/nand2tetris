#include "parser.hpp"

const std::regex Parser::A_command{"^@(\\d*)"};
const std::regex Parser::C_command{"(\\d*)=(\\d*);(\\d*)"};
const std::regex Parser::C_command_no_dest{"(\\d*);(\\d*)"};
const std::regex Parser::C_command_no_jump{"(\\d*)=(\\d*)"};

Parser::Parser(std::ifstream& input) : stream(std::move(input)) { };

bool Parser::hasMoreCommands() noexcept
{
    auto c = stream.peek();
    if (c == EOF) {
        return false;
    }
    return true;
};

void Parser::advance()
{
    std::string input;
    std::getline(stream, input);
    currentLine = sanitise(input);
};

std::experimental::optional<CommandType> const Parser::commandType()
{
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

const std::string& Parser::symbol() const
{
    return A_value;
};

const std::string& Parser::dest() const
{
    return C_dest;
};

const std::string& Parser::comp() const
{
    return C_comp;
};

const std::string& Parser::jump() const
{
    return C_jump;
};

std::string Parser::sanitise(std::string s)
{
    s.erase(std::remove_if(
                           s.begin(),
                           s.end(),
                           [](unsigned char x) { return std::isspace(x); }),
            s.end());
    return s.erase(s.find("//"));
};
