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

JackTokenizer::JackTokenizer(std::istream& in) : input(in) { };

std::shared_ptr<Token> JackTokenizer::nextToken(const std::string& input)
{
    if (isKeyword(input)) {
        auto token = validKeywords.find(input)->second;
        return std::make_shared<KeywordToken>(token);
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
    const std::regex pattern{"^?!([[:digit:]])[a-zA-Z0-9\\_]+"};
    return std::regex_match(input, pattern);
};
