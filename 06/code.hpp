#ifndef __code__
#define __code__

#include <string>
#include "parser.hpp"
#include "symbol_table.hpp"

class Code {
public:
    Code(Instruction& instr, SymbolTable& mapping);
    ~Code() = default;
    const std::string string() const;
private:
    SymbolTable& mappings;
    Instruction instruction;
    std::bitset<3> dest;
    std::bitset<7> comp;
    std::bitset<3> jump;
    std::bitset<15> value;
};

#endif
