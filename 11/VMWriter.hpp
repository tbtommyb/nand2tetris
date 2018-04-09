#ifndef __VMWriter__
#define __VMWriter__

#include <ostream>
#include <string>

struct Segment {
    enum Enum { CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP };
    static const std::string toString(const Segment::Enum& seg) {
        switch(seg) {
        case CONST:
            return "constant";
        case ARG:
            return "argument";
        case LOCAL:
            return "local";
        case STATIC:
            return "static";
        case THIS:
            return "this";
        case THAT:
            return "that";
        case POINTER:
            return "pointer";
        case TEMP:
            return "temp";
        }
    }
};

struct Command {
    enum Enum { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };
    static const std::string toString(const Command::Enum& cmd) {
        switch(cmd) {
        case ADD:
            return "add";
        case SUB:
            return "sub";
        case NEG:
            return "neg";
        case EQ:
            return "eq";
        case GT:
            return "gt";
        case LT:
            return "lt";
        case AND:
            return "and";
        case OR:
            return "or";
        case NOT:
            return "not";
        }
    };
};

class VMWriter {
public:
    VMWriter(std::ostream& out);
    void writePush(const Segment::Enum& segment, int index);
    void writePop(const Segment::Enum& segment, int index);
    void writeArithmetic(const Command::Enum& cmd);
    void writeLabel(const std::string& label);
    void writeGoto(const std::string& label);
    void writeIf(const std::string& label);
    void writeCall(const std::string& name, int nArgs);
    void writeFunction(const std::string& name, int nLocals);
    void writeReturn();
    bool write(const std::string& cmd, const std::string& arg1 = "", const std::string& arg2 = "");
private:
    std::ostream& out;
};

#endif
