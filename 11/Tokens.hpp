#ifndef __Tokens__
#define __Tokens__

#include <string>

class Token {
public:
    Token(int line) : lineNumber(line) { };
    virtual ~Token() = default;
    virtual const std::string toString() const { return ""; };
    virtual const std::string valToString() const { return ""; };
    int getLineNumber() const noexcept { return lineNumber; };
protected:
    int lineNumber;
};

class KeywordToken : public Token {
public:
    KeywordToken(std::string kw, int line) : Token(line), val(kw) { };
    const std::string getVal() const noexcept { return val; };
    const std::string toString() const override;
    const std::string valToString() const override { return val; };
private:
    std::string val;
};

class SymbolToken : public Token {
public:
    SymbolToken(char16_t symbol, int line) : Token(line), val(symbol) { };
    const char16_t getVal() const noexcept { return val; };
    const std::string toString() const override;
    const std::string valToString() const override;
private:
    char16_t val;
};

class IntConstToken : public Token {
public:
    IntConstToken(int16_t intVal, int line) : Token(line), val(intVal) { };
    const int16_t getVal() const noexcept { return val; };
    const std::string toString() const override;
    const std::string valToString() const override;
private:
    int16_t val;
};

class StringToken : public Token {
public:
    StringToken(const std::string& stringVal, int line) : Token(line), val(stringVal) { };
    const std::string getVal() const noexcept { return val; };
    const std::string toString() const override;
    const std::string valToString() const override { return val; };
private:
    std::string val;
};

class IdentifierToken : public Token {
public:
    IdentifierToken(const std::string& val, int line) : Token(line), val(val) { };
    const std::string getVal() const noexcept { return val; };
    const std::string toString() const override;
    const std::string valToString() const override { return val; };
private:
    std::string val;
};

#endif
