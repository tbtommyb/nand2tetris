#include <iostream>
#include "SymbolTable.hpp"

// TODOs
// cerr

void SymbolTable::startSubroutine()
{
    subroutineMap.clear();
    argumentCount = 0;
    varCount = 0;
};

Symbol SymbolTable::addSymbol(std::shared_ptr<Token> name, std::shared_ptr<Token> type, SymbolKind kind)
{
    return addSymbol(name->valToString(), type->valToString(), kind);
};

Symbol SymbolTable::addSymbol(const std::string& name, const std::string& type, const SymbolKind& kind)
{
    int count = 0;
    switch (kind) {
    case STATIC:
        count = staticCount++;
        break;
    case FIELD:
        count = fieldCount++;
        break;
    case ARGUMENT:
        count = argumentCount++;
        break;
    case VAR:
        count = varCount++;
        break;
    case NONE:
        std::cout << "Could not add " << name << " of type none." << std::endl;
    };

    Symbol entry = { type, kind, count };

    if (kind == STATIC || kind == FIELD) {
        classMap[name] = entry;
    } else {
        subroutineMap[name] = entry;
    }

    return entry;
};

const Symbol& SymbolTable::getSymbol(const std::string& name)
{
    try {
        return subroutineMap.at(name);
    } catch (std::out_of_range& e) {
        try {
            return classMap.at(name);
        } catch (std::out_of_range& e) {
            std::cout << "Could not find symbol " << name << std::endl;
            exit(1);
        }
    }
};

int SymbolTable::getCount(const SymbolKind& kind)
{
    switch (kind) {
    case STATIC:
        return staticCount;
        break;
    case FIELD:
        return fieldCount;
        break;
    case ARGUMENT:
        return argumentCount;
        break;
    case VAR:
        return varCount;
        break;
    case NONE:
        return 0;
    };
};
