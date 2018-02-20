#ifndef __code_writer__
#define __code_writer__

#include <istream>
#include <string>
#include "parser.hpp"

class CodeWriter {
public:
    CodeWriter(std::ostream&, std::string filename);
    void writePushPop(const Command&);
    void writeArithmetic(const Command&);
private:
    void writeConstant(int value);
    void writeFromSegment(std::string, int);
    void writeFromAddress(std::string, int);
    void writeToSegment(std::string, int);
    void writeToAddress(std::string, int);
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
    std::string filename;
};

#endif
