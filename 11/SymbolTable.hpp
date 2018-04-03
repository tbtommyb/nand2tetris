#ifndef __SymbolTable__
#define __SymbolTable__

#include <map>
#include <string>

enum SymbolKind = { NONE, STATIC, FIELD, ARGUMENT, VAR };

struct Symbol {
    std::string type;
    SymbolKind kind;
    int id;
};

class SymbolTable {
public:
    SymbolTable() = default;
    ~SymbolTable() = default;
    void addSymbol(const std::string& name, const std::string& type, const SymbolKind& kind);
    int getCount(const SymbolKind& kind);
    bool contains(const std::string& name);
private:
    std::map<std::string, Symbol> symbolMap;
    int staticCount, fieldCount, argumentCount, varCount;
};

#endif
