#include "Tokens.hpp"

const std::string KeywordToken::toString() const
{
    return "<keyword>" + val + "</keyword>";
};

const std::string SymbolToken::toString() const
{
    std::string sym(1, val);
    if (sym[0] == '<') {
        sym = "&lt;";
    }
    else if (sym[0] == '>') {
        sym = "&gt;";
    }
    else if (sym[0] == '\"') {
        sym = "&quot;";
    }
    else if (sym[0] == '&') {
        sym = "&amp;";
    }
    return "<symbol>" + sym + "</symbol>";
};

const std::string SymbolToken::valToString() const
{
    std::string symbol(1, val);
    return symbol;
};

const std::string IntConstToken::toString() const
{
    return "<integerConstant>" + std::to_string(val) + "</integerConstant>";
};

const std::string IntConstToken::valToString() const
{
    return std::to_string(val);
};

const std::string StringToken::toString() const
{
    return "<stringConstant>" + val.substr(1, val.size() - 2) + "</stringConstant>";
};

const std::string IdentifierToken::toString() const
{
    return "<identifier>" + val + "</identifier>";
};
