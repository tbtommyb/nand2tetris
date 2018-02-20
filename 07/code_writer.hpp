#ifndef __code_writer__
#define __code_writer__

#include <istream>
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
    void sum();
    void eq();
    void write(std::string arg);
    void writeEqualityBlock();
    std::ostream& out;
    int labelIndex;
};

#endif
