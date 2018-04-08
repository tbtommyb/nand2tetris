#ifndef __CompilationEngine__
#define __CompilationEngine__

#include <string>
#include <iostream>
#include "Tokens.hpp"
#include "JackTokenizer.hpp"
#include "CompilationError.hpp"
#include "SymbolTable.hpp"

class CompilationEngine {
public:
    CompilationEngine(TokenList& tokens, std::ostream&);
    ~CompilationEngine() = default;
    bool compile();
    bool compileClass();
    bool compileClassVarDec();
    std::shared_ptr<Token> readType();
    bool compileType();
    bool compileSubroutineDec();
    bool compileParameterList();
    bool compileVarDec();
    bool compileSubroutineBody();
    bool compileStatements();
    bool compileStatement();
    bool compileLet();
    bool compileIf();
    bool compileWhile();
    bool compileDo();
    bool compileReturn();
    bool compileExpression();
    bool compileTerm();
    bool compileSubroutineCall();
    bool compileExpressionList();
    bool compileOp();
    bool compileUnaryOp();
    bool compileKeywordConstant();
private:
    std::shared_ptr<KeywordToken> readKeyword(const std::vector<std::string>& options);
    bool writeKeyword(const std::string& kw);
    std::shared_ptr<IdentifierToken> readIdentifier();
    bool writeIdentifier();
    std::shared_ptr<SymbolToken> readSymbol(char16_t sym);
    bool writeSymbol(char16_t sym);
    bool writeIntConst();
    bool writeStringConst();
    bool tokenMatches(std::vector<std::string>);
    bool zeroOrOnce(const std::function<void(void)>&);
    bool zeroOrMany(const std::function<bool(void)>&);
    const std::string expected(const std::string&, const std::shared_ptr<Token>&);
    bool write(const std::string& val);
    std::vector<std::shared_ptr<Token>>::iterator token;
    std::ostream& out;
    SymbolTable symbolTable;
    int indentLevel, indent;
};

#endif