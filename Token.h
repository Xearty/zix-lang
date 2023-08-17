#pragma once

#include <vector>
#include <variant>
#include <string>

#define TOKEN_LIST(MACRO) \
    MACRO(INVALID) \
    MACRO(IDENTIFIER) \
    MACRO(FUNCTION) \
    MACRO(LPAREN) \
    MACRO(RPAREN) \
    MACRO(ARROW) \
    MACRO(LCURLY) \
    MACRO(RCURLY) \
    MACRO(RETURN) \
    MACRO(INT_LITERAL) \
    MACRO(SEMI_COLON) \
    MACRO(END_OF_FILE)

#define DECLARE_TOKENS(NAME) NAME,

enum class TokenType {
    TOKEN_LIST(DECLARE_TOKENS)
};

#undef DECLARE_TOKENS

using TokenData = std::variant<std::string, int>;
using TokenCollection = std::vector<struct Token>;

struct Token {
    TokenType type;
    TokenData data;
};

template <TokenType TType>
Token CreateToken() {
    Token token;
    token.type = TType;
    return token;
}

