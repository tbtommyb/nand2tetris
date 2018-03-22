#include <regex>
#include "JackTokenizer.hpp"

std::map<std::string, Keyword> validKeywords = {
    { "class", Keyword::CLASS },
    { "constructor", Keyword::CONSTRUCTOR },
    { "function", Keyword::FUNCTION },
    { "method", Keyword::METHOD },
    { "field", Keyword::FIELD },
    { "static", Keyword::STATIC },
    { "var", Keyword::VAR },
    { "int", Keyword::INT },
    { "char", Keyword::CHAR },
    { "boolean", Keyword::BOOLEAN },
    { "void", Keyword::VOID },
    { "true", Keyword::TRUE },
    { "false", Keyword::FALSE },
    { "null", Keyword::NULL_VAL },
    { "this", Keyword::THIS },
    { "let", Keyword::LET },
    { "do", Keyword::DO },
    { "if", Keyword::IF },
    { "else", Keyword::ELSE },
    { "while", Keyword::WHILE },
    { "return", Keyword::RETURN },
};

std::unordered_set<char16_t> validSymbols = {
    '{',
    '}',
    '(',
    ')',
    '[',
    ']',
    '.',
    ',',
    ';',
    '+',
    '-',
    '*',
    '/',
    '&',
    '|',
    '<',
    '>',
    '=',
    '~',
};

std::string KeywordToken::toString()
{
    return "<keyword>" + val + "</keyword>";
};

std::string SymbolToken::toString()
{
    std::string symbol(1, val);
    if (symbol[0] == '<') {
        symbol = "&lt;";
    }
    else if (symbol[0] == '>') {
        symbol = "&gt;";
    }
    else if (symbol[0] == '\"') {
        symbol = "&quot;";
    }
    else if (symbol[0] == '&') {
        symbol = "&amp;";
    }
    return "<symbol>" + symbol + "</symbol>";
};

std::string IntConstToken::toString()
{
    return "<integerConstant>" + std::to_string(val) + "</integerConstant>";
};

std::string StringToken::toString()
{
    return "<stringConstant>" + val.substr(1, val.size() - 2) + "</stringConstant>";
};

std::string IdentifierToken::toString()
{
    return "<identifier>" + val + "</identifier>";
};

JackTokenizer::JackTokenizer(std::istream& in) : input(in) { };

bool JackTokenizer::isCommentLine(std::string::iterator& it)
{
    if (*it != '/') {
        return false;
    }

    if (*++it == '/') {
        return true;
    }

    it--;
    return false;
};

bool JackTokenizer::skipCommentBlock(std::string::iterator& it)
{
    if (*it != '/') {
        return false;
    }

    if (*++it != '*') {
        it--;
        return false;
    }

    inCommentBlock = true;

    while (inCommentBlock) {
        if (*it++ == '*' && *it++ == '/') {
            inCommentBlock = false;
            it++;
        }
    }

    return true;
};

bool JackTokenizer::isRemainingChar(std::string::iterator& it)
{
    while (isspace(*it)) {
        it++;
    }

    if (isCommentLine(it)) {
        return false;
    }

    skipCommentBlock(it);

    if (it == currentLine.end()) {
        return false;
    }

    return true;
};

TokenList JackTokenizer::getTokenList()
{
    TokenList tokenList{};
    std::string currentEl;
    while(std::getline(input, currentLine)) {
        std::string::iterator it = currentLine.begin();
        while(isRemainingChar(it)) {
            std::string token{};
            if (isSymbol(*it)) {
                token.append(1, *it++);
            } else {
                while(it != currentLine.end() && !isspace(*it) && !isSymbol(*it)) {
                    token.append(1, *it++);
                }
            }
            tokenList.push_back(nextToken(token));
        }
    }

    return tokenList;
};

std::shared_ptr<Token> JackTokenizer::nextToken(const std::string& input)
{
    if (isKeyword(input)) {
        return std::make_shared<KeywordToken>(input);
    }
    if (isSymbol(input.c_str()[0])) {
        auto symbol = input.c_str()[0];
        return std::make_shared<SymbolToken>(symbol);
    }
    if (isInteger(input)) {
        return std::make_shared<IntConstToken>(std::stoi(input));
    }
    if (isString(input)) {
        return std::make_shared<StringToken>(input);
    }
    if (isIdentifier(input)) {
        return std::make_shared<IdentifierToken>(input);
    }
    return std::make_shared<Token>();
};

bool JackTokenizer::isKeyword(const std::string& input)
{
    return validKeywords.count(input);
};

bool JackTokenizer::isSymbol(char16_t input)
{
    return validSymbols.find(input) != validSymbols.end();
};

bool JackTokenizer::isInteger(const std::string& input)
{
    std::string::const_iterator it = input.begin();

    while (it != input.end() && isdigit(*it)) {++it; }

    if (it == input.end()) {
        int num = std::stoi(input);
        return num >= 0 && num <= 32767;
    }

    return false;
};

bool JackTokenizer::isString(const std::string& input)
{
    const std::regex pattern{"^\"[[:alpha:]]+\"$"};
    return std::regex_match(input, pattern);
};

bool JackTokenizer::isIdentifier(const std::string& input)
{
    const std::regex pattern{R"(^[a-zA-Z_]+[a-zA-Z0-9_]*)"};
    return std::regex_match(input, pattern);
};
