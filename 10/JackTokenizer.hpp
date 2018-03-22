#ifndef __JackTokenizer__
#define __JackTokenizer__

#include <istream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_set>
#include <vector>

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
    virtual std::string toString() { return "ILLEGAL TOKEN"; }
};

class KeywordToken : public Token {
public:
    KeywordToken(std::string kw) : val(kw) { };
    std::string toString() override;
private:
    std::string val;
};

class SymbolToken : public Token {
public:
    SymbolToken(char16_t symbol) : val(symbol) { };
    std::string toString() override;
private:
    char16_t val;
};

class IntConstToken : public Token {
public:
    IntConstToken(int16_t intVal) : val(intVal) { };
    std::string toString() override;
private:
    int16_t val;
};

class StringToken : public Token {
public:
    StringToken(const std::string& stringVal) : val(stringVal) { };
    std::string toString() override;
private:
    std::string val;
};

class IdentifierToken : public Token {
public:
    IdentifierToken(const std::string& val) : val(val) { };
    std::string toString() override;
private:
    std::string val;
};

typedef std::vector<std::shared_ptr<Token>> TokenList;

class JackTokenizer {
public:
    JackTokenizer(std::istream&);
    ~JackTokenizer() = default;
    std::vector<std::shared_ptr<Token>> getTokenList();
private:
    bool isRemainingChar(std::string::iterator&);
    bool isCommentLine(std::string::iterator&);
    bool skipCommentBlock(std::string::iterator&);
    std::shared_ptr<Token> nextToken(const std::string&);
    bool isKeyword(const std::string&);
    bool isSymbol(char16_t);
    bool isInteger(const std::string&);
    bool isString(const std::string&);
    bool isIdentifier(const std::string&);
    std::istream& input;
    std::string currentLine;
    bool inCommentBlock;
};

#endif
