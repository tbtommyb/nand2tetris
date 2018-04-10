#ifndef __CompilationEngine__
#define __CompilationEngine__

#include <string>
#include <iostream>
#include "Tokens.hpp"
#include "JackTokenizer.hpp"
#include "CompilationError.hpp"
#include "SymbolTable.hpp"
#include "VMWriter.hpp"

class CompilationEngine {
public:
    CompilationEngine(TokenList& tokens, std::ostream&);
    ~CompilationEngine() = default;
    bool compile();
    bool compileClass();
    bool compileClassVarDec();
    bool compileSubroutineDec();
    bool compileParameterList();
    bool compileVarDec();
    bool compileSubroutineBody(const std::shared_ptr<Token> name, const std::shared_ptr<Token> kw);
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
    int compileExpressionList();
    bool compileUnaryOp();
    bool compileKeywordConstant();
    bool compileIntConst();
    bool compileStringConst();
private:
    std::shared_ptr<SymbolToken> readOp();
    std::shared_ptr<Token> readType();
    std::shared_ptr<KeywordToken> readKeyword(const std::vector<std::string>& options);
    std::shared_ptr<IdentifierToken> readIdentifier();
    std::shared_ptr<SymbolToken> readSymbol(const std::vector<char16_t>& options);
    bool tokenMatches(std::vector<std::string>);
    bool zeroOrOnce(const std::function<void(void)>&);
    bool zeroOrMany(const std::function<bool(void)>&);
    const std::string expected(const std::string&, const std::shared_ptr<Token>&);
  const std::string newLabel();
    std::vector<std::shared_ptr<Token>>::iterator token;
    VMWriter vmWriter;
    std::string className;
    SymbolTable symbolTable;
    int labelCount;
};

#endif
