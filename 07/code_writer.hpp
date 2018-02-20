#ifndef __code_writer__
#define __code_writer__

#include <istream>
#include <string>
#include "parser.hpp"

class CodeWriter {
public:
    CodeWriter(std::ostream&);
    void output(const Command&);
private:
    void writeToStack(int value);
    void incrementStackPtr();
    void decrementStackPtr();
    void readStackToD();
    void readStackToA();
    void compare(std::string comparison);
    void write(std::string arg);
    void writeBootstrap();
    void equalityFn(std::string label, std::string comparison);
    std::ostream& out;
    int labelIndex;
};

#endif
