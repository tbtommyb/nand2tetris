#include <sstream>
#include <bitset>
#include <iostream>
#include <map>
#include "code.hpp"

std::map<std::string, std::string> destCodes = {
                                                { "", "000" },
                                                { "M", "001" },
                                                { "D", "010" },
                                                { "MD", "011" },
                                                { "A", "100" },
                                                { "AM", "101" },
                                                { "AD", "110" },
                                                { "AMD", "111" }
};

std::map<std::string, std::string> jumpCodes = {
                                                { "", "000" },
                                                { "JGT", "001" },
                                                { "JEQ", "010" },
                                                { "JGE", "011" },
                                                { "JLT", "100" },
                                                { "JNE", "101" },
                                                { "JLE", "110" },
                                                { "JMP", "111" }
 };

std::map<std::string, std::string> compCodes = {
                                                { "0", "0101010" },
                                                { "1", "0111111" },
                                                { "-1", "0111010" },
                                                { "D", "0001100" },
                                                { "A", "0110000" },
                                                { "!D", "0001101" },
                                                { "!A", "0110001" },
                                                { "-D", "0001111" },
                                                { "-A", "0110011" },
                                                { "D+1", "0011111" },
                                                { "A+1", "0110111" },
                                                { "D-1", "0001110" },
                                                { "A-1", "0110010" },
                                                { "D+A", "0000010" },
                                                { "D-A", "0010011" },
                                                { "A-D", "0000111" },
                                                { "D&A", "0000000" },
                                                { "D|A", "0010101" },
                                                { "M", "1110000" },
                                                { "!M", "1110001" },
                                                { "-M", "1110011" },
                                                { "M+1", "1110111" },
                                                { "M-1", "1110010" },
                                                { "D+M", "1000010" },
                                                { "D-M", "1010011" },
                                                { "M-D", "1000111" },
                                                { "D&M", "1000000" },
                                                { "D|M", "1010101" }
};

Code::Code(const Instruction& instr) : instruction(instr)
{
    // TODO ugly checking against type here
    if (instr.type == C_COMMAND) {
        dest = std::bitset<3>(destCodes[instr.dest]);
        comp = std::bitset<7>(destCodes[instr.comp]);
        jump = std::bitset<3>(destCodes[instr.jump]);
    } else {
        value = std::bitset<15>(std::stoi(instr.symbol));
    }
}

const std::string Code::string() const
{
    std::stringstream str;
    // TODO remove hardcoded strings here
    if (instruction.type == C_COMMAND) {
        str << "111" << comp.to_string() << dest.to_string() << jump.to_string();
    } else {
        str << "0" << value.to_string();
    }
    return str.str();
}

