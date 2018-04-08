#include <iostream>
#include "SymbolTable.hpp"

// TODOs
// cerr

std::map<std::string, SymbolKind::Enum> symbolMap = {
    { "static", SymbolKind::STATIC },
    { "field", SymbolKind::FIELD },
    { "argument", SymbolKind::ARGUMENT },
    { "var", SymbolKind::VAR }
};

void SymbolTable::startSubroutine()
{
    subroutineMap.clear();
    argumentCount = 0;
    varCount = 0;
};

Symbol SymbolTable::addSymbol(std::shared_ptr<Token> name, std::shared_ptr<Token> type, std::shared_ptr<Token> kind)
{
    return addSymbol(name->valToString(), type->valToString(), symbolMap.at(kind->valToString()));
};

Symbol SymbolTable::addSymbol(std::shared_ptr<Token> name, std::shared_ptr<Token> type, const SymbolKind::Enum& kind)
{
    return addSymbol(name->valToString(), type->valToString(), kind);
};

Symbol SymbolTable::addSymbol(const std::string& name, const std::string& type, const SymbolKind::Enum& kind)
{
    int count = 0;
    switch (kind) {
    case SymbolKind::STATIC:
        count = staticCount++;
        break;
    case SymbolKind::FIELD:
        count = fieldCount++;
        break;
    case SymbolKind::ARGUMENT:
        count = argumentCount++;
        break;
    case SymbolKind::VAR:
        count = varCount++;
        break;
    case SymbolKind::NONE:
        std::cout << "Could not add " << name << " of type none." << std::endl;
    };

    Symbol entry = { name, type, kind, count };

    if (kind == SymbolKind::STATIC || kind == SymbolKind::FIELD) {
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

int SymbolTable::getCount(const SymbolKind::Enum& kind)
{
    switch (kind) {
    case SymbolKind::STATIC:
        return staticCount;
        break;
    case SymbolKind::FIELD:
        return fieldCount;
        break;
    case SymbolKind::ARGUMENT:
        return argumentCount;
        break;
    case SymbolKind::VAR:
        return varCount;
        break;
    case SymbolKind::NONE:
        return 0;
    };
};
