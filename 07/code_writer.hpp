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
    void pop(std::string);
    void writeConstant(int);
    void writeFromSegment(std::string, int);
    void writeFromAddress(std::string, int);
    void writeToSegment(std::string, int);
    void writeToAddress(std::string, int);
    void incrementPointer(std::string);
    void decrementPointer(std::string);
    void writeToPointer(std::string, std::string);
    void loadFromPointer(std::string, std::string);
    void loadValue(std::string, std::string);
    void saveValueTo(std::string);
    void compare(std::string);
    void write(std::string);
    void writeBootstrap();
    void equalityFn(std::string label, std::string comparison);
    std::ostream& out;
    int labelIndex;
    std::string filename;
};

#endif
