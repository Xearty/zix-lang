#pragma once

#include "Result.h"
#include "Token.h"
#include "FileUtils.h"
#include "Utils.h"

#include <string_view>
#include <iostream>

enum class LexError {
    INVALID_TOKEN,
};

struct Lexer {
    const char* stream;
    int offset;
    char Peek(int lookAhead = 0) const {
        return stream[offset + lookAhead];
    }

    void Advance(int steps = 1) {
        offset += steps;
    }

    bool IsFinished() const {
        return !stream || Peek() == '\0';
    }

    bool Ensure(int span) const {
        for (int i = 0; i < span; ++i) {
            if (stream[offset + span] == '\0') {
                return false;
            }
        }
        return true;
    }

    const std::string_view GetView(int begin, int end) const {
        return std::string_view(stream + begin, end - begin);
    }

    int GetOffset() const {
        return offset;
    }
};

Lexer CreateLexer(const char* filename) {
    Lexer lexer;
    lexer.stream = FileUtils::ReadFile(filename);
    lexer.offset = 0;
    return lexer;
}

void DestroyLexer(Lexer& lexer) {
    delete lexer.stream;
}

void SkipWhitespace(Lexer& lexer) {
    while (std::isspace(lexer.Peek())) {
        lexer.Advance();
    }
}

template <TokenType T>
static constexpr bool AlwaysFalse = false;

template <TokenType TType>
bool TryParseToken(Lexer& lexer, Token& token) {
    // static_assert(AlwaysFalse<TType>);
    return false;
}

#define GENERATE_MONOSTATE_TOKEN_PARSING_FUNCTIONS(TOKEN_STRING, TOKEN_NAME) \
    template <>                                                              \
    bool TryParseToken<TokenType::TOKEN_NAME>(Lexer& lexer, Token& token) {  \
        const size_t len = STR_LIT_LEN(TOKEN_STRING);                        \
        for (size_t i = 0; i < len; ++i) {                                   \
            if (lexer.Peek(i) != (TOKEN_STRING)[i]) {                        \
                return false;                                                \
            }                                                                \
        }                                                                    \
        lexer.Advance(len);                                                  \
        token = CreateToken<TokenType::TOKEN_NAME>();                        \
        return true;                                                         \
    }

MONOSTATE_TOKEN_LIST(GENERATE_MONOSTATE_TOKEN_PARSING_FUNCTIONS)
#undef GENERATE_MONOSTATE_TOKEN_PARSING_FUNCTIONS

template <>
bool TryParseToken<TokenType::IDENTIFIER>(Lexer& lexer, Token& token) {
    if (!std::isalpha(lexer.Peek()) && lexer.Peek() != '_') {
        return false;
    }

    std::string value = "";

    while (std::isalnum(lexer.Peek()) || lexer.Peek() == '_') {
        value.push_back(lexer.Peek());
        lexer.Advance();
    }

    token = CreateTokenString<TokenType::IDENTIFIER>(std::move(value));
    return true;
}

template <>
bool TryParseToken<TokenType::INT_LITERAL>(Lexer& lexer, Token& token) {
    int value = 0;
    int litLen = 0;
    while (lexer.Peek(litLen) >= '0' && lexer.Peek(litLen) <= '9') {
        int digit = lexer.Peek(litLen) - '0';
        value = value * 10 + digit;
        ++litLen;
    }

    // Currently we disallow letters after the integer expression
    if (litLen == 0 || std::isalpha(lexer.Peek(litLen))) {
        return false;
    }

    lexer.Advance(litLen);
    token = CreateTokenInteger<TokenType::INT_LITERAL>(value);
    return true;
}

template <>
bool TryParseToken<TokenType::STR_LITERAL>(Lexer& lexer, Token& token) {
    if (lexer.Peek() != '"') {
        return false;
    }

    lexer.Advance();

    int begin = lexer.GetOffset();
    while (lexer.Peek() != '"') {
        lexer.Advance();
    }

    std::string value = std::string(lexer.GetView(begin, lexer.GetOffset()));
    lexer.Advance();

    token = CreateTokenString<TokenType::STR_LITERAL>(std::move(value));
    return true;
}

bool TryParseNextToken(Lexer& lexer, Token& token) {
#define TRY_PARSE_TOKEN(NAME) || TryParseToken<TokenType::NAME>(lexer, token)
    return false
        TOKEN_LIST(TRY_PARSE_TOKEN);
}

auto Tokenize(Lexer& lexer) -> Result<TokenCollection, LexError> {
    TokenCollection tokens;
    Token token;

    while (SkipWhitespace(lexer), !lexer.IsFinished()) {
        if (TryParseNextToken(lexer, token)) {
            tokens.push_back(token);
        } else {
            return Err(LexError::INVALID_TOKEN);
        }
    }

    tokens.push_back(CreateToken<TokenType::END_OF_FILE>());
    return Ok(std::move(tokens));
}

void PrintTokens(const std::vector<Token>& tokens) {
#define CASE(NAME)                                   \
    case TokenType::NAME:                            \
        std::cout << #NAME << ' ';                   \
        std::visit(DebugValueVisitor{}, token.data); \
        std::cout << '\n';                           \
    break;

    for (const auto& token : tokens) {
        switch (token.type) {
            TOKEN_LIST(CASE)
        }
    }
}

