#ifndef __JackTokenizer__
#define __JackTokenizer__

#include <istream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_set>
#include <vector>
#include "Tokens.hpp"

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
    int lineNumber;
};

#endif
