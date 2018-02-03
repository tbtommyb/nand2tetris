#include <exception>

class InvalidCommand : public std::exception
{
 public:
    InvalidCommand(std::string command) : command(command) {}
    ~InvalidCommand() noexcept {}
    virtual const char* what() const noexcept { return command.c_str(); }
private:
    std::string command;
};
