#include <regex>
#include "JackTokenizer.hpp"

std::map<std::string, Keyword> validKeywords = {
    { "class",       Keyword::CLASS },
    { "constructor", Keyword::CONSTRUCTOR },
    { "function",    Keyword::FUNCTION },
    { "method",      Keyword::METHOD },
    { "field",       Keyword::FIELD },
    { "static",      Keyword::STATIC },
    { "var",         Keyword::VAR },
    { "int",         Keyword::INT },
    { "char",        Keyword::CHAR },
    { "boolean",     Keyword::BOOLEAN },
    { "void",        Keyword::VOID },
    { "true",        Keyword::TRUE },
    { "false",       Keyword::FALSE },
    { "null",        Keyword::NULL_VAL },
    { "this",        Keyword::THIS },
    { "let",         Keyword::LET },
    { "do",          Keyword::DO },
    { "if",          Keyword::IF },
    { "else",        Keyword::ELSE },
    { "while",       Keyword::WHILE },
    { "return",      Keyword::RETURN },
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

JackTokenizer::JackTokenizer(std::istream& in) : input(in), lineNumber(0), multilineCommentBlock(false) { };

bool JackTokenizer::isCommentLine(std::string::iterator& it)
{
    if (multilineCommentBlock) {
        return currentLine.find("*/") == std::string::npos;
    }
    return *it == '/' && *std::next(it) == '/';
};

void JackTokenizer::skipCommentBlock(std::string::iterator& it)
{
    if (multilineCommentBlock) {
        while (!(*it == '*' && *std::next(it) == '/')) {
            it++;
        }
        multilineCommentBlock = false;
        std::advance(it, 2);
    }

    if (!(*it == '/' && *std::next(it) == '*')) {
        return;
    }

    std::advance(it, 2);

    while (!(*it == '*' && *std::next(it) == '/')) {
        it++;
        if (it == currentLine.end()) {
            multilineCommentBlock = true;
            return;
        }
    }

    std::advance(it, 2);
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
        lineNumber++;
        std::string::iterator it = currentLine.begin();
        while(isRemainingChar(it)) {
            std::string token{};
            if (isSymbol(*it)) {
                token.append(1, *it++);
            } else {
                // handle string constants
                if (*it == '\"') {
                    token.append(1, *it++);
                    while (it != currentLine.end() && *it != '\"') {
                        token.append(1, *it++);
                    }

                    token.append(1, *it++);
                } else {
                    while (it != currentLine.end() && !isspace(*it) && !isSymbol(*it)) {
                        token.append(1, *it++);
                    }
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
        return std::make_shared<KeywordToken>(input, lineNumber);
    }
    if (isSymbol(input.c_str()[0])) {
        auto symbol = input.c_str()[0];
        return std::make_shared<SymbolToken>(symbol, lineNumber);
    }
    if (isInteger(input)) {
        return std::make_shared<IntConstToken>(std::stoi(input), lineNumber);
    }
    if (isString(input)) {
        // strip double quotes
        const auto& str = input.substr(1, input.size() - 2);
        return std::make_shared<StringToken>(str, lineNumber);
    }
    if (isIdentifier(input)) {
        return std::make_shared<IdentifierToken>(input, lineNumber);
    }
    return std::make_shared<Token>(lineNumber);
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
    const std::regex pattern{R"(^\"[[:print:]]+\"$)"};
    return std::regex_match(input, pattern);
};

bool JackTokenizer::isIdentifier(const std::string& input)
{
    const std::regex pattern{R"(^[a-zA-Z_]+[a-zA-Z0-9_]*)"};
    return std::regex_match(input, pattern);
};
