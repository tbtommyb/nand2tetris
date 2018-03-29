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
    if (std::find(std::begin(expected), std::end(expected), (*token)->valToString()) == std::end(expected)) {
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
    if (std::find(std::begin(expected), std::end(expected), (*token)->valToString()) == std::end(expected)) {
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
    if ((*token)->valToString() != "var") { return false; }

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
    if ((*token)->valToString() != "{") { return false; }

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
    // A nicer way to do this would be good
    std::vector<std::string> expected{"let", "if", "else", "while", "do", "return"};
    if (std::find(std::begin(expected), std::end(expected), (*token)->valToString()) == std::end(expected)) {
        return false;
    }

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
    if ((*token)->valToString() != "let") { return false; }

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
    if ((*token)->valToString() != "if") { return false; }

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
    if ((*token)->valToString() != "while") { return false; }

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
    if ((*token)->valToString() != "do") { return false; }

    write("<doStatement>");
    indentLevel++;

    writeKeyword("do");

    compileSubroutineCall();

    writeKeyword(";");

    indentLevel--;
    write("</doStatement>");

    return true;
};

bool CompilationEngine::compileReturn()
{
    // 'return' expression? ';'
    if ((*token)->valToString() != "return") { return false; }

    write("<returnStatement>");
    indentLevel++;

    writeKeyword("return");

    zeroOrOnce([this] { return compileExpression(); });

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

              auto next = std::next(token, 1);
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

    writeIdentifier();
    oneOf(
          [this] {
              return writeSymbol('(') && compileExpressionList() && writeSymbol(')');
          },
          [this] {
              return writeSymbol('.') &&
                  writeIdentifier() &&
                  writeSymbol('(') &&
                  [this] {
                  // auto next = std::next(token, 1);
                  if ((*token)->valToString() != ")") {
                      return compileExpressionList();
                  }
                  return true;
              }() &&
                  writeSymbol(')');
          });

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

// bool CompilationEngine::compileStringConst()
// {
//     // '"' 

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
