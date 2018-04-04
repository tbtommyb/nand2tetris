#ifndef __SymbolTable__
#define __SymbolTable__

#include <map>
#include <string>

#include "Tokens.hpp"

enum SymbolKind { NONE, STATIC, FIELD, ARGUMENT, VAR };

struct Symbol {
    std::string type;
    SymbolKind kind;
    int id;
};

class SymbolTable {
public:
    SymbolTable() = default;
    ~SymbolTable() = default;
    void startSubroutine();
    Symbol addSymbol(std::shared_ptr<Token> name, std::shared_ptr<Token> type, SymbolKind kind);
    Symbol addSymbol(const std::string& name, const std::string& type, const SymbolKind& kind);
    const Symbol& getSymbol(const std::string& name);
    int getCount(const SymbolKind& kind);
private:
    std::map<std::string, Symbol> classMap;
    std::map<std::string, Symbol> subroutineMap;
    int staticCount = 0;
    int fieldCount = 0;
    int argumentCount = 0;
    int varCount = 0;
};

#endif
