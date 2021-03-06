#ifndef __parser__
#define __parser__

#include <regex>
#include <string>
#include <fstream>
#include "invalid_command.hpp"

enum CommandType { A_COMMAND, C_COMMAND, L_COMMAND };

struct Instruction {
    CommandType type;
    std::string symbol;
    std::string dest;
    std::string comp;
    std::string jump;
};

class Parser {
public:
    Parser(std::ifstream&);
    ~Parser() = default;
    bool hasMoreCommands() noexcept;
    void advance();
    const Instruction parse() throw(InvalidCommand);
private:
    CommandType const commandType() throw(InvalidCommand);
    const std::string& symbol() const;
    const std::string& dest() const;
    const std::string& comp() const;
    const std::string& jump() const;
    std::string sanitise(std::string);
    std::ifstream& stream;
    std::string currentLine, A_value, C_dest, C_comp, C_jump;
    static const std::regex A_command;
    static const std::regex L_command;
    static const std::regex C_command;
    static const std::regex C_command_no_dest;
    static const std::regex C_command_no_jump;
};

#endif
