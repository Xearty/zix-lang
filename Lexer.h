#pragma once

#include "Result.h"
#include "Token.h"
#include "FileUtils.h"

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

template <>
bool TryParseToken<TokenType::FUNCTION>(Lexer& lexer, Token& token) {
    if (lexer.Peek() == 'f' && lexer.Peek(1) == 'n') {
        lexer.Advance(2);
        token = CreateToken<TokenType::FUNCTION>();
        return true;
    }
    return false;
}

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

