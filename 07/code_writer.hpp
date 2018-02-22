#ifndef __code_writer__
#define __code_writer__

#include <istream>
#include <string>
#include "parser.hpp"

class CodeWriter {
public:
    CodeWriter(std::ostream& output, const std::string& filename);
    void writePushPop(const Command& command);
    void writeArithmetic(const Command& command);
private:
    void pop(const std::string& dest);
    void writeConstant(int value);
    void writeFromSegment(const std::string& segment, int index);
    void writeFromAddress(const std::string& address, int index);
    void writeToSegment(const std::string& segment, int index);
    void writeToAddress(const std::string& address, int index);
    void incrementPointer(const std::string& address);
    void decrementPointer(const std::string& address);
    void writeToPointer(const std::string& address, const std::string& val);
    void loadFromPointer(const std::string& address, const std::string& dest);
    void loadValue(const std::string& value, const std::string& dest);
    void saveValueTo(const std::string& address);
    void compare(const std::string& comparison);
    void write(const std::string& arg);
    void writeBootstrap();
    void equalityFn(const std::string& label, const std::string& comparison);
    std::ostream& out;
    int labelIndex;
    std::string filename;
};

#endif
