#include <iostream>
#include "SymbolTable.hpp"

// TODOs
// cerr

const std::string Symbol::toString() const
{
    return "<identifier kind='" +
        SymbolKind::toString(kind) +
        "' type='" + type +
        "' id='" + std::to_string(id) +
        "'>" + name + "</identifier>";
};

std::map<std::string, SymbolKind::Enum> symbolMap = {
    { "static", SymbolKind::STATIC },
    { "field", SymbolKind::FIELD },
    { "argument", SymbolKind::ARGUMENT },
    { "var", SymbolKind::VAR },
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

std::shared_ptr<Symbol> SymbolTable::getSymbol(const std::string& name)
{
    auto srIter = subroutineMap.find(name);
    if (srIter != subroutineMap.end()) {
        return std::make_shared<Symbol>(srIter->second);
    }
    auto cIter = classMap.find(name);
    if (cIter != classMap.end()) {
        return std::make_shared<Symbol>(cIter->second);
    }
    return nullptr;
};

int SymbolTable::getCount(const SymbolKind::Enum& kind)
{
    switch (kind) {
    case SymbolKind::STATIC:
        return staticCount;
    case SymbolKind::FIELD:
        return fieldCount;
    case SymbolKind::ARGUMENT:
        return argumentCount;
    case SymbolKind::VAR:
        return varCount;
    case SymbolKind::NONE:
        return 0;
    };
};
