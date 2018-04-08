#ifndef __SymbolTable__
#define __SymbolTable__

#include <map>
#include <string>
#include <sstream>

#include "Tokens.hpp"

struct SymbolKind {
    enum Enum { NONE, STATIC, FIELD, ARGUMENT, VAR };
    static const std::string toString(const SymbolKind::Enum& kind) {
        switch (kind) {
        case NONE:
            return "none";
        case STATIC:
            return "static";
        case FIELD:
            return "field";
        case ARGUMENT:
            return "argument";
        case VAR:
            return "var";
        }
    }
};

struct Symbol {
    std::string name;
    std::string type;
    SymbolKind::Enum kind;
    int id;
    const std::string toString() {
        return "<identifier kind='" + SymbolKind::toString(kind) + "' type='" + type + "' id='" + std::to_string(id) + "'>" + name + "</identifier>";
    }
};

class SymbolTable {
public:
    SymbolTable() = default;
    ~SymbolTable() = default;
    void startSubroutine();
    Symbol addSymbol(std::shared_ptr<Token> name, std::shared_ptr<Token> type, std::shared_ptr<Token> kind);
    Symbol addSymbol(std::shared_ptr<Token> name, std::shared_ptr<Token> type, const SymbolKind::Enum& kind);
    Symbol addSymbol(const std::string& name, const std::string& type, const SymbolKind::Enum& kind);
    const Symbol& getSymbol(const std::string& name);
    int getCount(const SymbolKind::Enum& kind);
private:
    std::map<std::string, Symbol> classMap;
    std::map<std::string, Symbol> subroutineMap;
    int staticCount = 0;
    int fieldCount = 0;
    int argumentCount = 0;
    int varCount = 0;
};

#endif
