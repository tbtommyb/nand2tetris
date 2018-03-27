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
        return t();
    } catch (const CompilationError& e) {
        return oneOf(Fs...);
    }
};

CompilationEngine::CompilationEngine(TokenList& tokens, std::ostream& out) : token(tokens.begin()), out(out), indentLevel(0), indent(2) { }

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

    writeKeyword("class");
    writeIdentifier();
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
    // TODO abstract this away
    std::vector<std::string> expected{"static", "field"};
    auto kwToken = std::dynamic_pointer_cast<KeywordToken>(*token);
    if (kwToken == nullptr ||
        (std::find(std::begin(expected), std::end(expected), kwToken->getVal()) == std::end(expected))) {
            return false;
        }

    write("<classVarDec>");
    indentLevel++;

    oneOf(
          [this] { return writeKeyword("static"); },
          [this] { return writeKeyword("field"); }
          );

    compileType();
    writeIdentifier();

    zeroOrMany([this] { return writeSymbol(',') && writeIdentifier(); });

    writeSymbol(';');

    indentLevel--;
    write("</classVarDec>");

    return true;
};

bool CompilationEngine::compileType()
{
    // 'int' | 'char' | 'boolean' | className
    return oneOf(
                 [this] { return writeKeyword("int"); },
                 [this] { return writeKeyword("char"); },
                 [this] { return writeKeyword("boolean"); },
                 [this] { return writeIdentifier(); }
                 );
};

bool CompilationEngine::compileSubroutineDec()
{
    // ('constructor' | 'function' | 'method')
    // ('void' | type) subroutineName '(' parameterList ')'
    // subroutineBody
    std::vector<std::string> expected{"constructor", "function", "method"};
    auto kwToken = std::dynamic_pointer_cast<KeywordToken>(*token);
    if (kwToken == nullptr ||
        (std::find(std::begin(expected), std::end(expected), kwToken->getVal()) == std::end(expected))) {
            return false;
        }

    write("<subroutineDec>");
    indentLevel++;

    oneOf(
          [this] { return writeKeyword("constructor"); },
          [this] { return writeKeyword("function"); },
          [this] { return writeKeyword("method"); }
          );

    oneOf(
          [this] { return writeKeyword("void"); },
          [this] { return compileType(); }
          );

    writeIdentifier();
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
    auto kwToken = std::dynamic_pointer_cast<KeywordToken>(*token);
    if (kwToken == nullptr) return false;

    write("<parameterList>");
    indentLevel++;

    zeroOrOnce([this] {
            return zeroOrOnce([this] {
                    return compileType() && writeIdentifier();
                        }) &&
                zeroOrMany([this] {return
                            writeSymbol(',') &&
                            compileType() &&
                            writeIdentifier();
                            });
        });

    indentLevel--;
    write("</parameterList>");

    return true;
};

bool CompilationEngine::compileVarDec()
{
    // 'var' type varName (',' varName)* ';'
    auto kwToken = std::dynamic_pointer_cast<KeywordToken>(*token);
    if (kwToken == nullptr || kwToken->getVal() != "var") return false;

    write("<varDec>");
    indentLevel++;

    writeKeyword("var");
    compileType();
    writeIdentifier();
    zeroOrMany([this] {
            return writeSymbol(',') && writeIdentifier();
        });
    writeSymbol(';');

    indentLevel--;
    write("</varDec>");

    return true;
};

bool CompilationEngine::compileSubroutineBody()
{
    // '{' varDec* statements '}'
    auto symbolToken = std::dynamic_pointer_cast<SymbolToken>(*token);
    if (symbolToken == nullptr || symbolToken->getVal() != '{') return false;

    write("<subroutineBody>");
    indentLevel++;

    writeSymbol('{');
    zeroOrMany([this] { return compileVarDec(); });
    // compileStatements();
    writeSymbol('}');

    indentLevel--;
    write("</subroutineBody");

    return true;
};

bool CompilationEngine::compileStatements()
{
    // statement*
    // TODO - how to do guard check?
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
    writeKeyword("let");
    writeIdentifier();

    zeroOrOnce([this] {
            return writeSymbol('[') && compileExpression() && writeSymbol(']');
        });

    writeSymbol('=');
    compileExpression();
    writeSymbol(';');

    return true;
};

bool CompilationEngine::compileIf()
{
    // 'if '(' expression ')' '{' statements '}' ('else' '{' statements '}')?
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

    return true;
};

bool CompilationEngine::compileWhile()
{
    // 'while' '(' expression ')' '{' statements '}'
    writeKeyword("while");

    writeSymbol('(');
    compileExpression();
    writeSymbol(')');

    writeSymbol('{');
    compileStatements();
    writeSymbol('}');

    return true;
};

bool CompilationEngine::compileDo()
{
    // 'do' subroutineCall ';'
    writeKeyword("do");

    compileSubroutineCall();

    writeKeyword(";");

    return true;
};

bool CompilationEngine::compileReturn()
{
    // 'return' expression? ';'
    writeKeyword("return");

    zeroOrOnce([this] { return compileExpression(); });

    writeSymbol(';');

    return true;
};

bool CompilationEngine::compileExpression()
{
    return true;
};

bool CompilationEngine::compileSubroutineCall()
{
    return true;
};

bool CompilationEngine::zeroOrOnce(std::function<void(void)> F)
{
    try {
        F();
        return true;
    } catch (const CompilationError& e) {
        return false;
    }
};

bool CompilationEngine::zeroOrMany(std::function<bool(void)> F)
{
    try {
        while(F()) { }
        return true;
    } catch (const CompilationError& e) {
        return false;
    }
};

bool CompilationEngine::writeKeyword(const std::string& ident)
{
    auto kwToken = std::dynamic_pointer_cast<KeywordToken>(*token);
    if (kwToken == nullptr) {
        std::stringstream ss{};
        ss << "keyword " << ident;
        throw CompilationError(expected(ss.str(), *token));
    }

    if (kwToken->getVal() == ident) {
        write(kwToken->toString());
        token++;
    } else {
        throw CompilationError(expected(ident, *token));
    }

    return true;
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

bool CompilationEngine::writeSymbol(char16_t sym)
{
    auto symbolToken = std::dynamic_pointer_cast<SymbolToken>(*token);
    std::string expect(1, sym);

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

const std::string CompilationEngine::expected(const std::string& expect, const std::shared_ptr<Token> got)
{
    std::stringstream ss{};
    ss << "l" << got->getLineNumber() << ": expected " << expect << "', received '" << got->valToString() << "'" << std::endl;
    return ss.str();
};

bool CompilationEngine::write(std::string val)
{
    std::string indentation(indentLevel * indent, ' ');
    out << indentation << val << std::endl;
    return out.good();
};
