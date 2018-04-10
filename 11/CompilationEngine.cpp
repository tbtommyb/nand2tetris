#include "CompilationEngine.hpp"

template<typename T>
bool oneOf(T t)
{
    try {
        return t();
    } catch (const CompilationError& e) {
        return false;
    }
};

template<typename T, typename... As>
bool oneOf(T t, As... Fs)
{
    try {
        if(t()) { return true; }
        return oneOf(Fs...);
    } catch (const CompilationError& e) {
        return oneOf(Fs...);
    }
};

template<typename T>
std::shared_ptr<Token> oneOfToken(T t)
{
    return t();
};

template<typename T, typename... As>
std::shared_ptr<Token> oneOfToken(T t, As... Fs)
{
    try {
        return t();
    } catch (const CompilationError& e) {
        return oneOfToken(Fs...);
    }
};

std::map<SymbolKind::Enum, Segment::Enum> kindSegmentMap = {
    { SymbolKind::STATIC, Segment::STATIC },
    { SymbolKind::FIELD, Segment::THIS },
    { SymbolKind::ARGUMENT, Segment::ARG },
    { SymbolKind::VAR, Segment::LOCAL },
};

std::map<char, std::string> opCommandMap = {
  { '+', "add" },
  { '-', "sub" },
  { '*', "call Math.multiple 2" },
  { '/', "call Math.divide 2" },
  { '<', "lt", },
  { '>', "gt" },
  { '=', "eq" },
  { '&', "and" },
  { '|', "or" },
};

CompilationEngine::CompilationEngine(TokenList& tokens, std::ostream& out) : token(tokens.begin()), vmWriter(out), out(out)
{
    symbolTable = SymbolTable{};
}

// Public compilation methods
// ==========================

bool CompilationEngine::compile()
{
    try {
        compileClass();
    } catch (const CompilationError& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    return true;
};

bool CompilationEngine::compileClass()
{
    // 'class' className '{' classVarDec* subroutineDec* '}'

    readKeyword({"class"});
    const auto& ident = readIdentifier();

    className = ident->valToString();

    readSymbol({'{'});

    zeroOrMany([this] { return compileClassVarDec(); });
    zeroOrMany([this] { return compileSubroutineDec(); });

    return true;
};

bool CompilationEngine::compileClassVarDec()
{
    // ('static' | 'field' ) type varName (',' varName)* ';'

    if (!tokenMatches({"static", "field"})) return false;

    const auto& kw = readKeyword({"static", "field"});
    const auto& type = readType();
    const auto& ident = readIdentifier();

    auto symbol = symbolTable.addSymbol(ident, type, kw);

    while(readSymbol({','}) != nullptr) {
        const auto& ident = readIdentifier();
        auto symbol = symbolTable.addSymbol(ident, type, kw);
    }

    readSymbol({';'});

    return true;
};

bool CompilationEngine::compileSubroutineDec()
{
    // ('constructor' | 'function' | 'method')
    // ('void' | type) subroutineName '(' parameterList ')'
    // subroutineBody

    if (!tokenMatches({"constructor", "function", "method"})) return false;

    symbolTable.startSubroutine();

    const auto& kw = readKeyword({"constructor", "function", "method"});
    if (kw->valToString() == "method") {
        symbolTable.addSymbol("this", className, SymbolKind::ARGUMENT);
    }

    const auto& returnType = oneOfToken(
               [this] { return readKeyword({"void"}); },
               [this] { return readType(); }
               );
    const auto& ident = readIdentifier();

    readSymbol({'('});
    compileParameterList();
    readSymbol({')'});

    compileSubroutineBody(ident, kw);

    return true;
};

bool CompilationEngine::compileParameterList()
{
    // ((type varName) (',' type varName)*)?

    if (!std::dynamic_pointer_cast<KeywordToken>(*token)) return false;

    if (readSymbol({')'}) == nullptr) {
        const auto& type = readType();
        const auto& ident = readIdentifier();

        symbolTable.addSymbol(ident, type, SymbolKind::ARGUMENT);

        while (readSymbol({','}) != nullptr) {
            const auto& type = readType();
            const auto& ident = readIdentifier();

            symbolTable.addSymbol(ident, type, SymbolKind::ARGUMENT);
        }
    }

    return true;
};

bool CompilationEngine::compileVarDec()
{
    // 'var' type varName (',' varName)* ';'

    if (!tokenMatches({"var"})) return false;

    const auto& kw = readKeyword({"var"});
    const auto& type = readType();
    const auto& ident = readIdentifier();

    symbolTable.addSymbol(ident, type, kw);

    while (readSymbol({','}) != nullptr) {
        const auto& ident = readIdentifier();
        symbolTable.addSymbol(ident, type, kw);
    }

    readSymbol({';'});

    return true;
};

bool CompilationEngine::compileSubroutineBody(const std::shared_ptr<Token> name, const std::shared_ptr<Token> kw)
{
    // '{' varDec* statements '}'

    if (!tokenMatches({"{"})) return false;

    readSymbol({'{'});

    zeroOrMany([this] { return compileVarDec(); });

    // TODO add 1 for methods
    vmWriter.writeFunction(className + "." + name->valToString(), symbolTable.getCount(SymbolKind::VAR));

    if (kw->valToString() == "constructor") {
        vmWriter.writePush(Segment::CONST, symbolTable.getCount(SymbolKind::FIELD));
        vmWriter.writeCall("Memory.alloc", 1);
        vmWriter.writePop(Segment::POINTER, 0);
    } else if (kw->valToString() == "method") {
        vmWriter.writePush(Segment::ARG, 0);
        vmWriter.writePop(Segment::POINTER, 0);
    }

    compileStatements();

    readSymbol({'}'});

    return true;
};

bool CompilationEngine::compileStatements()
{
    // statement*

    if (!tokenMatches({"let", "if", "else", "while", "do", "return"})) return false;

    zeroOrMany([this] { return compileStatement(); });

    return true;
};

bool CompilationEngine::compileStatement()
{
    // letStatement | ifStatement | whileStatement | doStatement | returnStatement

    return oneOf(
                 // [this] { return compileLet(); },
                 // [this] { return compileIf(); },
                 // [this] { return compileWhile(); },
                 [this] { return compileDo(); },
                 [this] { return compileReturn(); }
                 );
};

// bool CompilationEngine::compileLet()
// {
//     // 'let' varName ('[' expression ']')? '=' expression ';'

//     if (!tokenMatches({"let"})) return false;

//     readKeyword({"let"});
//     const auto& ident = symbolTable.getSymbol(readIdentifier()->valToString());

//     // zeroOrOnce([this] {
//     //         return writeSymbol('[') && compileExpression() && writeSymbol(']');
//     //     });

//     readSymbol({'='});
//     compileExpression();

//     auto segment = kindSegmentMap.at(ident->kind);
//     vmWriter.writePop(segment, ident->id);

//     readSymbol({';'});

//     return true;
// };

// bool CompilationEngine::compileIf()
// {
//     // 'if '(' expression ')' '{' statements '}' ('else' '{' statements '}')?

//     if (!tokenMatches({"if"})) return false;

//     write("<ifStatement>");
//     indentLevel++;

//     writeKeyword("if");

//     writeSymbol('(');
//     compileExpression();
//     writeSymbol(')');

//     writeSymbol('{');
//     compileStatements();
//     writeSymbol('}');

//     zeroOrOnce([this] {
//             return writeKeyword("else") && writeSymbol('{') &&
//                 compileStatements() && writeSymbol('}');
//         });

//     indentLevel--;
//     write("</ifStatement>");

//     return true;
// };

// bool CompilationEngine::compileWhile()
// {
//     // 'while' '(' expression ')' '{' statements '}'

//     if (!tokenMatches({"while"})) return false;

//     write("<whileStatement>");
//     indentLevel++;

//     writeKeyword("while");

//     writeSymbol('(');
//     compileExpression();
//     writeSymbol(')');

//     writeSymbol('{');
//     compileStatements();
//     writeSymbol('}');

//     indentLevel--;
//     write("</whileStatement>");

//     return true;
// };

bool CompilationEngine::compileDo()
{
    // 'do' subroutineCall ';'

    if (!tokenMatches({"do"})) return false;

    readKeyword({"do"});

    compileSubroutineCall();

    vmWriter.writePop(Segment::TEMP, 0);

    return true;
};

bool CompilationEngine::compileReturn()
{
    // 'return' expression? ';'

    if (!tokenMatches({"return"})) return false;

    readKeyword({"return"});

    if ((*token)->valToString() != ";") {
        zeroOrOnce([this] { return compileExpression(); });
    } else {
      vmWriter.writePush(Segment::CONST, 0);
    }

    vmWriter.writeReturn();

    readSymbol({';'});

    return true;
};

bool CompilationEngine::compileExpression()
{
    // term (op term)*

    compileTerm();

    zeroOrMany([this] {
        const auto& op = readOp();
        if (op != nullptr) {
          compileTerm();
          vmWriter.write(opCommandMap.at(op->getVal()));
          token++;
          return true;
        }
        return false;
      });

    return true;
};

bool CompilationEngine::compileTerm()
{
    // integerConstant | stringConstant | keywordConstant | varName |
    // varName '[' expression ']' | subroutineCall |
    // '(' expression ')' | unaryOp term

    oneOf(
          [this] { return compileIntConst(); },
          [this] { return compileStringConst(); },
          [this] { return compileKeywordConstant(); },
          // [this] {
          //     auto tok = std::dynamic_pointer_cast<IdentifierToken>(*token);
          //     if (tok == nullptr) { return false; }

          //     auto next = std::next(token);
          //     if ((*next)->valToString() == "[") {
          //         return writeIdentifier() && writeSymbol('[') && compileExpression() && writeSymbol(']');
          //     }
          //     if (((*next)->valToString() == ".") || ((*next)->valToString() == "(")) {
          //         return compileSubroutineCall();
          //     } else {
          //         return writeIdentifier();
          //     }
          // },
          [this] {
              if (readSymbol({'('}) != nullptr) {
                  compileExpression();
                  readSymbol({')'});
                  return true;
              }
              return false;
          },
          [this] { return compileUnaryOp() && compileTerm(); }
          );

    return true;
};

bool CompilationEngine::compileSubroutineCall()
{
    // subroutineName '(' expressionList ')' | (className | varName)
    // '.' subroutineName '(' expressionList ')'

    std::string typeName{};
    std::string name{};
    int numArgs = 0;

    auto next = std::next(token);
    if ((*next)->valToString() == ".") {
        const auto& ident = readIdentifier();
        readSymbol({'.'});

        auto symbol = symbolTable.getSymbol(ident->valToString());

        if (symbol.get() == nullptr) {
            // it's a class
            typeName = ident->valToString();
        } else {
            typeName = symbol->type;
        }
        const auto& methodName = readIdentifier();
        name = typeName + "." + methodName->valToString();

    } else {
      const auto& ident = readIdentifier();
      numArgs = 1;
      vmWriter.writePush(Segment::POINTER, 0);
      name = className + "." + ident->valToString();
    }

    readSymbol({'('});
    if (readSymbol({')'}) == nullptr) {
        numArgs += compileExpressionList();
        readSymbol({')'});
    }

    vmWriter.writeCall(name, numArgs);

    return true;
};

int CompilationEngine::compileExpressionList()
{
    // (expression (',' expression)* )?

    int numArgs = 0;
    compileExpression();
    numArgs++;
    while (readSymbol({','}) != nullptr) {
        compileExpression();
        numArgs++;
    }
    return numArgs;
};

bool CompilationEngine::compileUnaryOp()
{
    // '-' | '~'

    const auto& op = readSymbol({'~', '-'});
    vmWriter.write(opCommandMap.at(op->getVal()));
    return true;
};

bool CompilationEngine::compileKeywordConstant()
{
    // 'true'| 'false' | 'null' | 'this'

    const auto& kw = readKeyword({"true", "false", "null", "this"});
    if (kw->valToString() == "true") {
      vmWriter.writePush(Segment::CONST, 1);
        vmWriter.write("neg");
    } else if (kw->valToString() == "false" || kw->valToString() == "null") {
      vmWriter.writePush(Segment::CONST, 0);
    } else {
      vmWriter.writePush(Segment::POINTER, 0);
    }
    return true;
};

bool CompilationEngine::compileIntConst()
{
    const auto intTok = std::dynamic_pointer_cast<IntConstToken>(*token);
    if (intTok == nullptr) {
        throw CompilationError(expected("intConst", *token));
    }

    vmWriter.writePush(Segment::CONST, std::stoi(intTok->valToString()));

    token++;

    return true;
};

bool CompilationEngine::compileStringConst()
{
    auto strTok = std::dynamic_pointer_cast<StringToken>(*token);
    if (strTok == nullptr) {
        throw CompilationError(expected("stringConst", *token));
    }

    auto string = strTok->toString();
    vmWriter.writePush(Segment::CONST, string.length());
    vmWriter.writeCall("String.new", 1);
    for (const char& c : string) {
      vmWriter.writePush(Segment::CONST, int(c));
        vmWriter.writeCall("String.appendChar", 2);
    }
    token++;

    return true;
};

// Private helper methods
// ======================

std::shared_ptr<SymbolToken> CompilationEngine::readOp()
{
    // '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='

    return readSymbol({'+', '-', '*', '/', '&', '|', '<', '>', '='});
};

std::shared_ptr<Token> CompilationEngine::readType()
{
    // 'int' | 'char' | 'boolean' | className

    return oneOfToken(
                      [this] { return readKeyword({"int", "char", "boolean"}); },
                      [this] { return readIdentifier(); }
                      );
};

std::shared_ptr<KeywordToken> CompilationEngine::readKeyword(const std::vector<std::string>& options)
{
    auto kwToken = std::dynamic_pointer_cast<KeywordToken>(*token);
    if (kwToken == nullptr) {
        throw CompilationError(expected("keyword", *token));
    }

    for (auto& option : options) {
        if(kwToken->getVal() == option) {
            token++;
            return kwToken;
        }
    }

    throw CompilationError(expected("keyword", *token));
};

bool CompilationEngine::writeKeyword(const std::string& kw)
{
    auto kwToken = std::dynamic_pointer_cast<KeywordToken>(*token);
    if (kwToken == nullptr) {
        std::stringstream ss{};
        ss << "keyword '" << kw;
        throw CompilationError(expected(ss.str(), *token));
    }

    if (kwToken->getVal() == kw) {
        write(kwToken->toString());
        token++;
    } else {
        throw CompilationError(expected(kw, *token));
    }

    return true;
};

std::shared_ptr<IdentifierToken> CompilationEngine::readIdentifier()
{
    auto identToken = std::dynamic_pointer_cast<IdentifierToken>(*token);
    if (identToken == nullptr) {
        throw CompilationError(expected("identifier", *token));
    }

    token++;
    return identToken;
};

bool CompilationEngine::writeIdentifier()
{
    auto identToken = std::dynamic_pointer_cast<IdentifierToken>(*token);
    if (identToken == nullptr) {
        throw CompilationError(expected("identifier", *token));
    }

    write(identToken->toString());
    token++;

    return true;
};

std::shared_ptr<SymbolToken> CompilationEngine::readSymbol(const std::vector<char16_t>& options)
{
    auto symbolToken = std::dynamic_pointer_cast<SymbolToken>(*token);
    if (symbolToken == nullptr) {
        return nullptr;
    }

    for (auto& option : options) {
        if (symbolToken->getVal() == option) {
            token++;
            return symbolToken;
        }
    }

    throw CompilationError(expected("symbol", *token));
};

bool CompilationEngine::writeSymbol(char16_t sym)
{
    std::string expect(1, sym);

    auto symbolToken = std::dynamic_pointer_cast<SymbolToken>(*token);
    if (symbolToken == nullptr) {
        std::stringstream ss{};
        ss << "symbol '" << expect;
        throw CompilationError(expected(ss.str(), *token));
    }

    if (symbolToken->getVal() == sym) {
        write(symbolToken->toString());
        token++;
    } else {
        throw CompilationError(expected(expect, *token));
    }

    return true;
};

bool CompilationEngine::writeIntConst()
{
    auto intToken = std::dynamic_pointer_cast<IntConstToken>(*token);
    if (intToken == nullptr) {
        throw CompilationError(expected("intConst", *token));
    }

    write(intToken->toString());
    token++;

    return true;
};

bool CompilationEngine::writeStringConst()
{
    auto stringToken = std::dynamic_pointer_cast<StringToken>(*token);
    if (stringToken == nullptr) {
        throw CompilationError(expected("stringConst", *token));
    }

    write(stringToken->toString());
    token++;

    return true;
};

bool CompilationEngine::tokenMatches(std::vector<std::string> options)
{
    return std::find(std::begin(options), std::end(options), (*token)->valToString()) != std::end(options);
};

bool CompilationEngine::zeroOrOnce(const std::function<void(void)>& F)
{
    try {
        F();
        return true;
    } catch (const CompilationError& e) {
        return false;
    }
};

bool CompilationEngine::zeroOrMany(const std::function<bool(void)>& F)
{
    try {
        while(F()) { }
        return true;
    } catch (const CompilationError& e) {
        return false;
    }
};

const std::string CompilationEngine::expected(const std::string& expect, const std::shared_ptr<Token>& got)
{
    std::stringstream ss{};
    ss << "l" << got->getLineNumber() << ": expected " << expect << "', received '" << got->valToString() << "'" << std::endl;
    return ss.str();
};

bool CompilationEngine::write(const std::string& val)
{
    std::string indentation(indentLevel * indent, ' ');
    out << indentation << val << std::endl;
    return out.good();
};
