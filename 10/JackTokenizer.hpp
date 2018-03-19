#ifndef __JackTokenizer__
#define __JackTokenizer__

#include <istream>
#include <string>
#include <map>
#include <unordered_set>

enum class Keyword {
    CLASS,
    CONSTRUCTOR,
    FUNCTION,
    METHOD,
    FIELD,
    STATIC,
    VAR,
    INT,
    CHAR,
    BOOLEAN,
    VOID,
    TRUE,
    FALSE,
    NULL_VAL,
    THIS,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN
};

class Token {
public:
    Token() { };
    virtual ~Token() = default;
};

class KeywordToken : public Token {
public:
    KeywordToken(Keyword kw) : val(kw) { };
private:
    Keyword val;
};

class SymbolToken : public Token {
public:
    SymbolToken(char16_t symbol) : val(symbol) { };
private:
    char16_t val;
};

class IntConstToken : public Token {
public:
    IntConstToken(int16_t intVal) : val(intVal) { };
private:
    int16_t val;
};

class StringToken : public Token {
public:
    StringToken(const std::string& stringVal) : val(stringVal) { };
private:
    std::string val;
};

class IdentifierToken : public Token {
public:
    IdentifierToken(const std::string& identifier) : val(identifier) { };
private:
    std::string val;
};

class JackTokenizer {
public:
    JackTokenizer(std::istream&);
    ~JackTokenizer() = default;
    std::shared_ptr<Token> nextToken(const std::string&);
private:
    bool isKeyword(const std::string&);
    bool isSymbol(char16_t);
    bool isInteger(const std::string&);
    bool isString(const std::string&);
    bool isIdentifier(const std::string&);
    std::istream& input;
};

#endif
