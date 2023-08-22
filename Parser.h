#pragma once

#include "CommonTypes.h"
#include "Token.h"
#include "ASTNode.h"

#include <cassert>

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

    ASTNodeRef ParseVariableDeclaration() {
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
        if (auto decl = ParseFunctionDeclaration()) return decl;
        else if (auto decl = ParseVariableDeclaration()) return decl;
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
        return parser.ParseTopStatements();
    }

private:
    TokenCollection m_Tokens;
    int m_Current = 0;
};

static ASTNodeRef Parse(const TokenCollection& tokens) {
    return Parser::Parse(tokens);
}

