#pragma once

#include <vector>
#include <variant>
#include <string>
#include <iostream>

#define TOKEN_LIST(MACRO) \
    MACRO(INVALID)        \
    MACRO(FUNCTION)       \
    MACRO(FOR)            \
    MACRO(LET)            \
    MACRO(EQUALS)         \
    MACRO(LPAREN)         \
    MACRO(RPAREN)         \
    MACRO(ARROW)          \
    MACRO(LCURLY)         \
    MACRO(RCURLY)         \
    MACRO(RETURN)         \
    MACRO(INT_LITERAL)    \
    MACRO(STR_LITERAL)    \
    MACRO(SEMI_COLON)     \
    MACRO(COLON)          \
    MACRO(DOT)            \
    MACRO(COMMA)          \
    MACRO(IDENTIFIER)     \
    MACRO(END_OF_FILE)

// Tokens that don't have any more
// state except for the type of token
// (used for generating parsing functions)
#define MONOSTATE_TOKEN_LIST(MACRO) \
    MACRO("fn", FUNCTION)           \
    MACRO("for", FOR)               \
    MACRO("let", LET)               \
    MACRO("=", EQUALS)              \
    MACRO("(", LPAREN)              \
    MACRO(")", RPAREN)              \
    MACRO("->", ARROW)              \
    MACRO("{", LCURLY)              \
    MACRO("}", RCURLY)              \
    MACRO("return", RETURN)         \
    MACRO(";", SEMI_COLON)          \
    MACRO(":", COLON)               \
    MACRO(".", DOT)                 \
    MACRO(",", COMMA)


#define DECLARE_TOKENS(NAME) NAME,

enum class TokenType {
    TOKEN_LIST(DECLARE_TOKENS)
};

#undef DECLARE_TOKENS

using TokenData = std::variant<std::monostate, std::string, int>;
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

template <TokenType TType, typename ValueType>
Token CreateTokenData(ValueType value) {
    Token token = CreateToken<TType>();
    token.data = std::move(value);
    return token;
}

struct DebugValueVisitor {
    void operator()(const std::string& value) {
        std::cout << "(String: " << value << ')';
    }

    void operator()(int value) {
        std::cout << "(Integer: " << value << ')';
    }

    void operator()(std::monostate value) {}
};

