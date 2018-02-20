#ifndef __parser__
#define __parser__

#include <string>

enum CommandType {
                  C_ARITHMETIC,
                  C_PUSH,
                  C_POP,
                  C_LABEL,
                  C_GOTO,
                  C_IF,
                  C_FUNCTION,
                  C_RETURN,
                  C_CALL
};

struct Command {
    CommandType type;
    std::string arg1;
    int arg2;
};

class Parser {
public:
    Parser(std::istream&);
    ~Parser() = default;
    bool hasMoreCommands() noexcept;
    void advance();
    Command parse();
private:
    std::istream& source;
    std::string currentCommand;
    std::string sanitise(std::string);
};

#endif
