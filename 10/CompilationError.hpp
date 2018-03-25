#ifndef __CompilationError__
#define __CompilationError__

#include <string>

class CompilationError {
public:
    CompilationError(const char* msg) : msg(msg) { }
    CompilationError(std::string msg) : CompilationError(msg.c_str()) { }
    const char* what() const noexcept { return msg; }
private:
    const char* msg;
};

#endif
