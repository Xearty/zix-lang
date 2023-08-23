#pragma once

#include <vector>
#include <variant>
#include <string>
#include <iostream>

#include "CommonTypes.h"

#define TOKEN_LIST(MACRO) \
    MACRO(INVALID)        \
    MACRO(FUNCTION)       \
    MACRO(FOR)            \
    MACRO(LET)            \
    MACRO(ARROW)          \
    MACRO(RETURN)         \
    MACRO(EQUALS)         \
    MACRO(PLUS)           \
    MACRO(MINUS)          \
    MACRO(STAR)           \
    MACRO(SLASH)          \
    MACRO(LPAREN)         \
    MACRO(RPAREN)         \
    MACRO(LCURLY)         \
    MACRO(RCURLY)         \
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
    MACRO("->", ARROW)              \
    MACRO("return", RETURN)         \
    MACRO("+", PLUS)                \
    MACRO("-", MINUS)               \
    MACRO("*", STAR)                \
    MACRO("/", SLASH)               \
    MACRO("(", LPAREN)              \
    MACRO(")", RPAREN)              \
    MACRO("{", LCURLY)              \
    MACRO("}", RCURLY)              \
    MACRO(";", SEMI_COLON)          \
    MACRO(":", COLON)               \
    MACRO(".", DOT)                 \
    MACRO(",", COMMA)


#define DECLARE_TOKENS(NAME) NAME,

enum class TokenType {
    TOKEN_LIST(DECLARE_TOKENS)
};

#undef DECLARE_TOKENS

using TokenData = std::variant<std::monostate, String, int>;
using TokenCollection = Vector<struct Token>;

struct Location {
    int line;
    int column;
};

struct Token {
    TokenType type;
    TokenData data;
    Location location;
};

template <TokenType TType>
Token CreateToken(Location location) {
    Token token;
    token.type = TType;
    token.location = location;
    return token;
}

template <TokenType TType, typename ValueType>
Token CreateTokenData(ValueType value, Location location) {
    Token token = CreateToken<TType>(location);
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

static const char* GetTokenName(TokenType token) {
    #define RETURN_TOKEN_NAME(NAME) case TokenType::NAME: return #NAME;
    switch (token) {
        TOKEN_LIST(RETURN_TOKEN_NAME);
    }
    return nullptr;
};

