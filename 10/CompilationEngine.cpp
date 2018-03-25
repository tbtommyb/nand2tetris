#include "CompilationEngine.hpp"

#include <cstdio>

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
    writeSymbol('}');

    indentLevel--;
    write("</class>");

    return true;
};

void CompilationEngine::writeKeyword(const std::string& ident)
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
};

void CompilationEngine::writeIdentifier()
{
    auto identToken = std::dynamic_pointer_cast<IdentifierToken>(*token);
    if (identToken == nullptr) {
        throw CompilationError(expected("identifier", *token));
    }

    write(identToken->toString());
    token++;
    return;
};

void CompilationEngine::writeSymbol(char16_t sym)
{
    auto symbolToken = std::dynamic_pointer_cast<SymbolToken>(*token);
    if (symbolToken == nullptr) {
        std::stringstream ss{};
        ss << "symbol " << sym;
        throw CompilationError(expected(ss.str(), *token));
    }

    if (symbolToken->getVal() == sym) {
        write(symbolToken->toString());
        token++;
    } else {
        std::string expect(1, sym);
        throw CompilationError(expected(expect, *token));
    }
};

const std::string CompilationEngine::expected(const std::string& expect, const std::shared_ptr<Token> got)
{
    std::stringstream ss{};
    ss << "l" << got->getLineNumber() << ": expected " << expect << ", received " << got->valToString() << std::endl;
    return ss.str();
};

bool CompilationEngine::write(std::string val)
{
    std::string indentation(indentLevel * indent, ' ');
    out << indentation << val << std::endl;
    return out.good();
};
