#ifndef __symbol_table__
#define __symbol_table__

#include <string>
#include <map>

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable() = default;
    void addEntry(std::string, unsigned int);
    void addVariable(std::string);
    bool contains(std::string);
    unsigned int getAddress(std::string);
private:
    std::map<std::string, unsigned int> mapping;
    static unsigned int currentVariableAddress;
};

#endif
