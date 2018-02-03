#ifndef __code__
#define __code__

#include <string>
#include "parser.hpp"

class Code {
public:
    Code(const Instruction& instr);
    ~Code() = default;
    const std::string string() const;
private:
    Instruction instruction;
    std::bitset<3> dest;
    std::bitset<7> comp;
    std::bitset<3> jump;
    std::bitset<15> value;
};

#endif
