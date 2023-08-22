#pragma once

#include "CommonTypes.h"
#include "Token.h"
#include "ASTNode.h"

class Parser {
public:
    explicit Parser(const TokenCollection& tokens)
        : m_Tokens(tokens)
    {}

    bool Consume(TokenType type) {
        if (m_Tokens[m_Current].type == type) {
            ++m_Current;
            return true;
        }
        return false;
    }

    const Token& GetPrevToken() const {
        return m_Tokens[m_Current - 1];
    }

    ExpressionPtr ParseExpression() {
        if (Consume(TokenType::INT_LITERAL)) {
            const Token& initialValueToken = GetPrevToken();
            int initialValue = std::get<int>(initialValueToken.data);
            return MakeShared<IntegerLiteralExpression>(initialValue);

        } else if (Consume(TokenType::IDENTIFIER)) {
            const Token& initialValueToken = GetPrevToken();
            String initialValue = std::get<String>(initialValueToken.data);
            return MakeShared<IdentifierExpression>(initialValue);
        }

        return nullptr;
    }

    ExpressionPtr ParseVariableDeclaration() {
        if (Consume(TokenType::LET) && Consume(TokenType::IDENTIFIER)) {
            auto identifierName = std::get<String>(GetPrevToken().data);
            if (Consume(TokenType::EQUALS)) {
                if (auto initialValueExpr = ParseExpression()) {
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
            const Token& paramNameIdent = GetPrevToken();
            String paramName = std::get<String>(paramNameIdent.data);

            if (Consume(TokenType::COLON) && Consume(TokenType::IDENTIFIER)) {
                const Token& paramTypeIdent = GetPrevToken();
                String paramType = std::get<String>(paramTypeIdent.data);
                params.push_back(FuncParam{ std::move(paramName), std::move(paramType) });
                return true;
            }
        }
        return false;
    }

    ExpressionPtr ParseFunctionDeclaration() {
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

        auto ParseBody = [&]() -> ExpressionPtr {
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
            const Token& identToken = GetPrevToken();
            String functionIdent = std::get<String>(identToken.data);

            Vector<FuncParam> params;
            if (ParseParameterList(params)) {
                if (Consume(TokenType::ARROW) && Consume(TokenType::IDENTIFIER)) {
                    const Token& returnTypeIdent = GetPrevToken();
                    String returnType = std::get<String>(returnTypeIdent.data);

                    if (auto body = ParseBody()) {
                        return MakeShared<FunctionDeclaration>(functionIdent, params, returnType, body);
                    }
                }
            }
        }

        return nullptr;
    }

    ExpressionPtr ParseTopStatement() {
        if (auto decl = ParseFunctionDeclaration()) return decl;
        else if (auto decl = ParseVariableDeclaration()) return decl;
        return nullptr;
    }

    ExpressionPtr ParseTopStatements() {
        Vector<ExpressionPtr> statements;
        while (auto statement = ParseTopStatement()) {
            statements.push_back(std::move(statement));
        }
        return MakeShared<TopStatements>(statements);
    }

    static ExpressionPtr Parse(const TokenCollection& tokens) {
        Parser parser(tokens);
        return parser.ParseTopStatements();
    }

private:
    TokenCollection m_Tokens;
    int m_Current = 0;
};

static ExpressionPtr Parse(const TokenCollection& tokens) {
    return Parser::Parse(tokens);
}

