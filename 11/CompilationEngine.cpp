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

CompilationEngine::CompilationEngine(TokenList& tokens, std::ostream& out) : token(tokens.begin()), out(out), indentLevel(0), indent(2)
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

    write("<class>");
    indentLevel++;

    readKeyword({"class"});
    const auto& ident = readIdentifier();

    write(symbolTable.create(ident, SymbolKind::CLASS).toString());

    writeSymbol('{');

    zeroOrMany([this] { return compileClassVarDec(); });
    zeroOrMany([this] { return compileSubroutineDec(); });

    writeSymbol('}');

    indentLevel--;
    write("</class>");

    return true;
};

bool CompilationEngine::compileClassVarDec()
{
    // ('static' | 'field' ) type varName (',' varName)* ';'

    if (!tokenMatches({"static", "field"})) return false;

    write("<classVarDec>");
    indentLevel++;

    const auto& kw = readKeyword({"static", "field"});
    const auto& type = readType();
    const auto& ident = readIdentifier();

    auto symbol = symbolTable.addSymbol(ident, type, kw);
    write(symbol.toString());

    while(readSymbol(',') != nullptr) {
        const auto& ident = readIdentifier();
        auto symbol = symbolTable.addSymbol(ident, type, kw);
        write(symbol.toString());
    }

    readSymbol(';');

    indentLevel--;
    write("</classVarDec>");

    return true;
};

std::shared_ptr<Token> CompilationEngine::readType()
{
    // 'int' | 'char' | 'boolean' | className

    return oneOfToken(
                      [this]() { return readKeyword({"int", "char", "boolean"}); },
                      [this]() { return readIdentifier(); }
                      );
};

bool CompilationEngine::compileSubroutineDec()
{
    // ('constructor' | 'function' | 'method')
    // ('void' | type) subroutineName '(' parameterList ')'
    // subroutineBody

    if (!tokenMatches({"constructor", "function", "method"})) return false;

    symbolTable.startSubroutine();

    write("<subroutineDec>");
    indentLevel++;

    readKeyword({"constructor", "function", "method"});
    oneOfToken(
               [this] { return readKeyword({"void"}); },
               [this] { return readType(); }
               );
    const auto& ident = readIdentifier();

    write(symbolTable.create(ident, SymbolKind::SUBROUTINE).toString());

    writeSymbol('(');
    compileParameterList();
    writeSymbol(')');

    compileSubroutineBody();

    indentLevel--;
    write("</subroutineDec>");

    return true;
};

bool CompilationEngine::compileParameterList()
{
    // ((type varName) (',' type varName)*)?

    if (!std::dynamic_pointer_cast<KeywordToken>(*token)) return false;

    write("<parameterList>");
    indentLevel++;

    if (readSymbol(')') == nullptr) {
        const auto& type = readType();
        const auto& ident = readIdentifier();

        auto symbol = symbolTable.addSymbol(ident, type, SymbolKind::ARGUMENT);
        write(symbol.toString());

        while (readSymbol(',') != nullptr) {
            const auto& type = readType();
            const auto& ident = readIdentifier();

            auto symbol = symbolTable.addSymbol(ident, type, SymbolKind::ARGUMENT);
            write(symbol.toString());
        }
    }

    indentLevel--;
    write("</parameterList>");

    return true;
};

bool CompilationEngine::compileVarDec()
{
    // 'var' type varName (',' varName)* ';'

    if (!tokenMatches({"var"})) return false;

    write("<varDec>");
    indentLevel++;

    const auto& kw = readKeyword({"var"});
    const auto& type = readType();
    const auto& ident = readIdentifier();

    auto symbol = symbolTable.addSymbol(ident, type, kw);
    write(symbol.toString());

    while(readSymbol(',') != nullptr) {
        const auto& ident = readIdentifier();
        auto symbol = symbolTable.addSymbol(ident, type, kw);
        write(symbol.toString());
    }

    readSymbol(';');

    indentLevel--;
    write("</varDec>");

    return true;
};

bool CompilationEngine::compileSubroutineBody()
{
    // '{' varDec* statements '}'

    if (!tokenMatches({"{"})) return false;

    write("<subroutineBody>");
    indentLevel++;

    writeSymbol('{');
    zeroOrMany([this] { return compileVarDec(); });
    compileStatements();
    writeSymbol('}');

    indentLevel--;
    write("</subroutineBody");

    return true;
};

bool CompilationEngine::compileStatements()
{
    // statement*

    if (!tokenMatches({"let", "if", "else", "while", "do", "return"})) return false;

    write("<statements>");
    indentLevel++;

    zeroOrMany([this] { return compileStatement(); });

    indentLevel--;
    write("</statements>");

    return true;
};

bool CompilationEngine::compileStatement()
{
    // letStatement | ifStatement | whileStatement | doStatement | returnStatement

    return oneOf(
                 [this] { return compileLet(); },
                 [this] { return compileIf(); },
                 [this] { return compileWhile(); },
                 [this] { return compileDo(); },
                 [this] { return compileReturn(); }
                 );
};

bool CompilationEngine::compileLet()
{
    // 'let' varName ('[' expression ']')? '=' expression ';'

    if (!tokenMatches({"let"})) return false;

    write("<letStatement>");
    indentLevel++;

    writeKeyword("let");
    writeIdentifier();

    zeroOrOnce([this] {
            return writeSymbol('[') && compileExpression() && writeSymbol(']');
        });

    writeSymbol('=');
    compileExpression();
    writeSymbol(';');

    indentLevel--;
    write("</letStatement>");

    return true;
};

bool CompilationEngine::compileIf()
{
    // 'if '(' expression ')' '{' statements '}' ('else' '{' statements '}')?

    if (!tokenMatches({"if"})) return false;

    write("<ifStatement>");
    indentLevel++;

    writeKeyword("if");

    writeSymbol('(');
    compileExpression();
    writeSymbol(')');

    writeSymbol('{');
    compileStatements();
    writeSymbol('}');

    zeroOrOnce([this] {
            return writeKeyword("else") && writeSymbol('{') &&
                compileStatements() && writeSymbol('}');
        });

    indentLevel--;
    write("</ifStatement>");

    return true;
};

bool CompilationEngine::compileWhile()
{
    // 'while' '(' expression ')' '{' statements '}'

    if (!tokenMatches({"while"})) return false;

    write("<whileStatement>");
    indentLevel++;

    writeKeyword("while");

    writeSymbol('(');
    compileExpression();
    writeSymbol(')');

    writeSymbol('{');
    compileStatements();
    writeSymbol('}');

    indentLevel--;
    write("</whileStatement>");

    return true;
};

bool CompilationEngine::compileDo()
{
    // 'do' subroutineCall ';'

    if (!tokenMatches({"do"})) return false;

    write("<doStatement>");
    indentLevel++;

    writeKeyword("do");

    compileSubroutineCall();

    writeSymbol(';');

    indentLevel--;
    write("</doStatement>");

    return true;
};

bool CompilationEngine::compileReturn()
{
    // 'return' expression? ';'

    if (!tokenMatches({"return"})) return false;

    write("<returnStatement>");
    indentLevel++;

    writeKeyword("return");

    if ((*token)->valToString() != ";") {
        zeroOrOnce([this] { return compileExpression(); });
    };

    writeSymbol(';');

    indentLevel--;
    write("</returnStatement>");

    return true;
};

bool CompilationEngine::compileExpression()
{
    // term (op term)*

    write("<expression>");
    indentLevel++;

    compileTerm();

    zeroOrMany([this] { return compileOp() && compileTerm(); });

    indentLevel--;
    write("</expression>");

    return true;
};

bool CompilationEngine::compileTerm()
{
    // integerConstant | stringConstant | keywordConstant | varName |
    // varName '[' expression ']' | subroutineCall |
    // '(' expression ')' | unaryOp term

    write("<term>");
    indentLevel++;

    oneOf(
          [this] { return writeIntConst(); },
          [this] { return writeStringConst(); },
          [this] { return compileKeywordConstant(); },
          [this] {
              auto tok = std::dynamic_pointer_cast<IdentifierToken>(*token);
              if (tok == nullptr) { return false; }

              auto next = std::next(token);
              if ((*next)->valToString() == "[") {
                  return writeIdentifier() && writeSymbol('[') && compileExpression() && writeSymbol(']');
              }
              if (((*next)->valToString() == ".") || ((*next)->valToString() == "(")) {
                  return compileSubroutineCall();
              } else {
                  return writeIdentifier();
              }
          },
          [this] { return writeSymbol('(') && compileExpression() && writeSymbol(')'); },
          [this] { return compileUnaryOp() && compileTerm(); }
          );

    indentLevel--;
    write("</term>");

    return true;
};

bool CompilationEngine::compileSubroutineCall()
{
    // subroutineName '(' expressionList ')' | (className | varName)
    // '.' subroutineName '(' expressionList ')'

    write("<subroutineCall>");
    indentLevel++;

    auto next = std::next(token);
    if ((*next)->valToString() == ".") {
        const auto& ident = readIdentifier();
        // TODO check for existing var in symboltable, use class if not found
        write(symbolTable.create(ident, SymbolKind::CLASS).toString());
        readSymbol('.');
    }

    const auto& ident = readIdentifier();
    write(symbolTable.create(ident, SymbolKind::SUBROUTINE).toString());

    writeSymbol('(');
    if (readSymbol(')') == nullptr) {
        compileExpressionList();
        writeSymbol(')');
    }

    indentLevel--;
    write("</subroutineCall>");

    return true;
};

bool CompilationEngine::compileExpressionList()
{
    // (expression (',' expression)* )?

    write("<expressionList>");
    indentLevel++;

    zeroOrOnce([this] {
            return compileExpression() &&
                zeroOrMany([this] {
                        return writeSymbol(',') && compileExpression();
                    });
        });

    indentLevel--;
    write("</expressionList>");

    return true;
};

bool CompilationEngine::compileOp()
{
    // '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='

    return oneOf(
                 [this] { return writeSymbol('+'); },
                 [this] { return writeSymbol('-'); },
                 [this] { return writeSymbol('*'); },
                 [this] { return writeSymbol('/'); },
                 [this] { return writeSymbol('&'); },
                 [this] { return writeSymbol('|'); },
                 [this] { return writeSymbol('<'); },
                 [this] { return writeSymbol('>'); },
                 [this] { return writeSymbol('='); }
                 );
};

bool CompilationEngine::compileUnaryOp()
{
    // '-' | '~'

    return oneOf(
                 [this] { return writeSymbol('~'); },
                 [this] { return writeSymbol('-'); }
                 );
};

bool CompilationEngine::compileKeywordConstant()
{
    // 'true'| 'false' | 'null' | 'this'

    return oneOf(
                 [this] { return writeKeyword("true"); },
                 [this] { return writeKeyword("false"); },
                 [this] { return writeKeyword("null"); },
                 [this] { return writeKeyword("this"); }
                 );
};

// Private helper methods
// ======================

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

std::shared_ptr<SymbolToken> CompilationEngine::readSymbol(char16_t sym)
{
    std::string expect(1, sym);
    std::stringstream ss{};
    ss << "symbol '" << expect;

    auto symbolToken = std::dynamic_pointer_cast<SymbolToken>(*token);
    if (symbolToken == nullptr) {
        return nullptr;
    }

    if (symbolToken->getVal() != sym) {
        return nullptr;
    }

    token++;
    return symbolToken;
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
