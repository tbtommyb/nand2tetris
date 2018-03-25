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
    // bool compileClassVarDec();
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
    void writeKeyword(const std::string& kw);
    void writeIdentifier();
    void writeSymbol(char16_t sym);
    const std::string expected(const std::string&, std::shared_ptr<Token>);
    bool write(std::string val);
    std::vector<std::shared_ptr<Token>>::iterator token;
    std::ostream& out;
    int indentLevel, indent;
};

#endif
