#include "symbol_table.hpp"
#include <iostream>

unsigned int SymbolTable::currentVariableAddress = 0x0010;

SymbolTable::SymbolTable() : mapping{}
{
    addEntry("SP", 0x0000);
    addEntry("LCL", 0x0001);
    addEntry("ARG", 0x0002);
    addEntry("THIS", 0x0003);
    addEntry("THAT", 0x0004);
    addEntry("SCREEN", 0x4000);
    addEntry("KBD", 0x6000);

    for(std::size_t i = 0; i < 16; i++) {
        addEntry("R" + std::to_string(i), 0x000 + i);
    }
}

void SymbolTable::addEntry(std::string symbol, unsigned int address)
{
    mapping.emplace(symbol, address);
}

void SymbolTable::addVariable(std::string symbol)
{
    addEntry(symbol, currentVariableAddress++);
}

bool SymbolTable::contains(std::string symbol)
{
    auto search = mapping.find(symbol);
    return search != mapping.end();
}

unsigned int SymbolTable::getAddress(std::string symbol)
{
    return mapping[symbol];
}
