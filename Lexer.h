#pragma once

#include "Result.h"
#include "Token.h"
#include "FileUtils.h"
#include "Utils.h"

#include <string_view>
#include <iostream>

enum class LexError {
    NO_STREAM,
    INVALID_TOKEN,
};

struct Lexer {
    explicit Lexer(const char* filename)
        : m_Stream(FileUtils::ReadFile(filename))
    {}

    ~Lexer() {
        delete m_Stream;
    }

    char Peek(int lookAhead = 0) const {
        return m_Stream[m_Offset + lookAhead];
    }

    void Advance(int steps = 1) {
        for (int i = 0; i < steps; ++i) {
            if (Peek() == '\n') {
                m_Location.line++;
                m_Location.column = 0;
            } else {
                m_Location.column++;
            }
        }
        m_Offset += steps;
    }

    void Done() {
        m_IsDone = true;
    }

    bool IsDone() const {
        return m_IsDone;
    }

    bool HasStream() const {
        return (bool)m_Stream;
    }

    const std::string_view GetView(int begin, int end) const {
        return std::string_view(m_Stream + begin, end - begin);
    }

    int GetOffset() const {
        return m_Offset;
    }

    const Location& GetLocation() const {
        return m_Location;
    }

    const char* GetStream() const {
        return m_Stream;
    }

private:
    const char* m_Stream = nullptr;
    int m_Offset = 0;
    Location m_Location = { 1, 1 };
    bool m_IsDone = false;
};

void SkipWhitespace(Lexer& lexer) {
    while (std::isspace(lexer.Peek())) {
        lexer.Advance();
    }
}

template <TokenType T>
static constexpr bool AlwaysFalse = false;

template <TokenType TType>
bool TryParseToken(Lexer& lexer, Token& token) {
    static_assert(AlwaysFalse<TType>);
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
        token = CreateToken<TokenType::TOKEN_NAME>(lexer.GetLocation());     \
        return true;                                                         \
    }

MONOSTATE_TOKEN_LIST(GENERATE_MONOSTATE_TOKEN_PARSING_FUNCTIONS)
#undef GENERATE_MONOSTATE_TOKEN_PARSING_FUNCTIONS

template <>
bool TryParseToken<TokenType::IDENTIFIER>(Lexer& lexer, Token& token) {
    if (!std::isalpha(lexer.Peek()) && lexer.Peek() != '_') {
        return false;
    }

    String value = "";

    while (std::isalnum(lexer.Peek()) || lexer.Peek() == '_') {
        value.push_back(lexer.Peek());
        lexer.Advance();
    }

    token = CreateTokenData<TokenType::IDENTIFIER>(std::move(value), lexer.GetLocation());
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
    token = CreateTokenData<TokenType::INT_LITERAL>(value, lexer.GetLocation());
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

    String value = std::string(lexer.GetView(begin, lexer.GetOffset()));
    lexer.Advance();

    token = CreateTokenData<TokenType::STR_LITERAL>(std::move(value), lexer.GetLocation());
    return true;
}

template <>
bool TryParseToken<TokenType::END_OF_FILE>(Lexer& lexer, Token& token) {
    bool reachedEOF =  !lexer.HasStream() || lexer.Peek() == '\0';
    if (reachedEOF) {
        token = CreateToken<TokenType::END_OF_FILE>(lexer.GetLocation());
        lexer.Done();
    }

    return reachedEOF;
}

// Not sure if I need this but it will hopefully get optimized out
// Currently exists because of the static assert
template <>
bool TryParseToken<TokenType::INVALID>(Lexer& lexer, Token& token) {
    return false;
}

bool TryParseNextToken(Lexer& lexer, Token& token) {
#define TRY_PARSE_TOKEN(NAME) || TryParseToken<TokenType::NAME>(lexer, token)
    return false
        TOKEN_LIST(TRY_PARSE_TOKEN);
}

auto Tokenize(Lexer& lexer) -> Result<TokenCollection, LexError> {
    if (!lexer.HasStream()) {
        return Err(LexError::NO_STREAM);
    }

    TokenCollection tokens;
    Token token;

    while (!lexer.IsDone()) {
        SkipWhitespace(lexer);

        if (TryParseNextToken(lexer, token)) {
            tokens.push_back(token);
        } else {
            const char* tokenName = GetTokenName(tokens.back().type);
            std::cout << "Last parsed token: " << tokenName << std::endl;
            return Err(LexError::INVALID_TOKEN);
        }
    }

    return Ok(std::move(tokens));
}

void PrintTokens(const Vector<Token>& tokens) {
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

