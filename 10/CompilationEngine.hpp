#ifndef __CompilationEngine__
#define __CompilationEngine__

#include <sstream>
#include <string>
#include <iostream>
#include "Tokens.hpp"
#include "JackTokenizer.hpp"
#include "CompilationError.hpp"

class CompilationEngine {
public:
    CompilationEngine(TokenList& tokens, std::ostream&);
    ~CompilationEngine() = default;
    bool compile();
    bool compileClass();
    bool compileClassVarDec();
    bool compileType();
    // bool compileSubroutine();
    // bool compileParameterList();
    // bool compileVarDec();
    // bool compileStatements();
    // bool compileDo();
    // bool compileLet();
    // bool compileWhile();
    // bool compileIf();
    // bool compileExpression();
    // bool compileTerm();
    // bool compileExpressionList();
private:
    bool writeKeyword(const std::string& kw);
    bool writeIdentifier();
    bool writeSymbol(char16_t sym);
    bool zeroOrOnce(std::function<void(void)>);
    bool zeroOrMany(std::function<void(void)>);
    const std::string expected(const std::string&, std::shared_ptr<Token>);
    bool write(std::string val);
    std::vector<std::shared_ptr<Token>>::iterator token;
    std::ostream& out;
    int indentLevel, indent;
};

#endif
