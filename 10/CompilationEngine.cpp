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

    compileClassVarDec();

    writeSymbol('}');

    indentLevel--;
    write("</class>");

    return true;
};

bool CompilationEngine::compileClassVarDec()
{
    // ('static' | 'field' ) type varName (',' varName)* ';'

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
    return oneOf(
                 [this] { return writeKeyword("int"); },
                 [this] { return writeKeyword("char"); },
                 [this] { return writeKeyword("boolean"); },
                 [this] { return writeIdentifier(); }
                 );
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

bool CompilationEngine::zeroOrMany(std::function<void(void)> F)
{
    try {
        while(zeroOrOnce(F)) { }
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
    ss << "l" << got->getLineNumber() << ": expected '" << expect << "', received '" << got->valToString() << "'" << std::endl;
    return ss.str();
};

bool CompilationEngine::write(std::string val)
{
    std::string indentation(indentLevel * indent, ' ');
    out << indentation << val << std::endl;
    return out.good();
};
