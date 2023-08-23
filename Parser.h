#pragma once

#include "CommonTypes.h"
#include "Token.h"
#include "ASTNode.h"

#include <cassert>
#include <algorithm>

#define TRY_PARSE(AST_NODE) if (auto node = Parse##AST_NODE()) return node

class Parser {
public:
    explicit Parser(TokenCollection tokens)
        : m_Tokens(std::move(tokens))
    {}

    bool Consume(TokenType type) {
        if (m_Tokens[m_Current].type == type) {
            ++m_Current;
            return true;
        }
        return false;
    }

    [[nodiscard]] const Token& GetCurrentToken() const {
        return m_Tokens[m_Current];
    }

    [[nodiscard]] const Token& GetPrevToken() const {
        assert(m_Current > 0);
        return m_Tokens[m_Current - 1];
    }

    ASTNodeRef ParseExpression() {
        if (Consume(TokenType::INT_LITERAL)) {
            int initialValue = std::get<int>(GetPrevToken().data);
            return MakeShared<IntegerLiteralExpression>(initialValue);

        } else if (Consume(TokenType::IDENTIFIER)) {
            String initialValue = std::get<String>(GetPrevToken().data);
            return MakeShared<IdentifierExpression>(initialValue);
        }

        return nullptr;
    }

    bool ConsumeOneOf(const Vector<TokenType>& tokens) {
        return std::any_of(tokens.begin(), tokens.end(),[this](TokenType token) {
            return Consume(token);
        });
    }

    ASTNodeRef ParseAdditiveExpression() {
        auto left = ParseMultiplicativeExpression();
        while (ConsumeOneOf({ TokenType::PLUS, TokenType::MINUS })) {
            TokenType op = GetPrevToken().type;
            if (auto right = ParseMultiplicativeExpression()) {
                left = MakeShared<BinaryExpression>(op, left, right);
            } else {
                return nullptr;
            }
        }

        return left;
    }

    ASTNodeRef ParseMultiplicativeExpression() {
        ASTNodeRef left = ParseExpression();
        while (ConsumeOneOf({ TokenType::STAR, TokenType::SLASH })) {
            TokenType op = GetPrevToken().type;
            if (auto right = ParseExpression()) {
                left = MakeShared<BinaryExpression>(op, left, right);
            } else {
                return nullptr;
            }
        }

        return left;
    }

    ASTNodeRef ParseAssignmentExpression() {
        TRY_PARSE(AdditiveExpression);
        return nullptr;
    }

    ASTNodeRef ParseVariableDeclaration() {
        if (Consume(TokenType::LET) && Consume(TokenType::IDENTIFIER)) {
            auto identifierName = std::get<String>(GetPrevToken().data);
            if (Consume(TokenType::EQUALS)) {
                if (auto initialValueExpr = ParseAdditiveExpression()) {
                    if (Consume(TokenType::SEMI_COLON)) {
                        return MakeShared<VariableDeclaration>(identifierName, initialValueExpr);
                    }
                }

            }
        }

        return nullptr;
    }

    bool ParseParameter(Vector<FuncParam>& params) {
        while (Consume(TokenType::IDENTIFIER)) {
            String paramName = std::get<String>(GetPrevToken().data);

            if (Consume(TokenType::COLON) && Consume(TokenType::IDENTIFIER)) {
                String paramType = std::get<String>(GetPrevToken().data);
                params.push_back(FuncParam{ std::move(paramName), std::move(paramType) });
                return true;
            }
        }
        return false;
    }

    ASTNodeRef ParseFunctionDeclaration() {
        auto ParseParameterList = [&](Vector<FuncParam>& params) -> bool {
            if (Consume(TokenType::LPAREN)) {
                while (ParseParameter(params)) {
                    if (!Consume(TokenType::COMMA)) {
                        break;
                    }
                }

                if (Consume(TokenType::RPAREN)) {
                    return true;
                }
            }
            return false;
        };

        auto ParseBody = [&]() -> ASTNodeRef {
            if (Consume(TokenType::LCURLY)) {
                if (auto statements = ParseTopStatements()) {
                    if (Consume(TokenType::RCURLY)) {
                        return statements;
                    }
                }

            }
            return nullptr;
        };

        if (Consume(TokenType::FUNCTION) && Consume(TokenType::IDENTIFIER)) {
            String functionIdent = std::get<String>(GetPrevToken().data);

            Vector<FuncParam> params;
            if (ParseParameterList(params)) {
                if (Consume(TokenType::ARROW) && Consume(TokenType::IDENTIFIER)) {
                    String returnType = std::get<String>(GetPrevToken().data);

                    if (auto body = ParseBody()) {
                        return MakeShared<FunctionDeclaration>(functionIdent, params, returnType, body);
                    }
                }
            }
        }

        return nullptr;
    }

    ASTNodeRef ParseTopStatement() {
        TRY_PARSE(FunctionDeclaration);
        TRY_PARSE(VariableDeclaration);
        return nullptr;
    }

    ASTNodeRef ParseTopStatements() {
        Vector<ASTNodeRef> statements;
        while (auto statement = ParseTopStatement()) {
            statements.push_back(std::move(statement));
        }
        return MakeShared<TopStatements>(statements);
    }

    static ASTNodeRef Parse(const TokenCollection& tokens) {
        Parser parser(tokens);
        auto statements = parser.ParseTopStatements();

        const Token& currentToken = parser.GetCurrentToken();
        if(currentToken.type != TokenType::END_OF_FILE) {
            std::cout << "Unexpected token: " << GetTokenName(currentToken.type);
            std::cout << " (" << currentToken.location.line << ":" << currentToken.location.column << ")";
            std::cout << std::endl;
        }
        return statements;
    }

private:
    TokenCollection m_Tokens;
    int m_Current = 0;
};

static ASTNodeRef Parse(const TokenCollection& tokens) {
    return Parser::Parse(tokens);
}

