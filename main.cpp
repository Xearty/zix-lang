#include <iostream>
#include <memory>
#include "Result.h"
#include "Lexer.h"
#include "Utils.h"

#include "CommonTypes.h"

struct ASTNode {};

struct FuncParam {
    String name;
    String type;
};

#define NO_PROPERTIES(MACRO)

#define TOP_STATEMENTS_PROPERTIES(MACRO) \
    MACRO(Vector<ExpressionPtr>, Statements)

#define FOR_EXPRESSION_PROPERTIES(MACRO) \
    MACRO(ExpressionPtr, Initialization) \
    MACRO(ExpressionPtr, Condition)      \
    MACRO(ExpressionPtr, Increment)

#define FUNCTION_DECLARATION_PROPERTIES(MACRO) \
    MACRO(String, Name)                        \
    MACRO(Vector<FuncParam>, Parameters)       \
    MACRO(String, ReturnType)                  \
    MACRO(ExpressionPtr, Body)

#define EXPRESSION_LIST(MACRO)                                  \
    MACRO(TopStatements, TOP_STATEMENTS_PROPERTIES)             \
    MACRO(ForStatement, FOR_EXPRESSION_PROPERTIES)              \
    MACRO(FunctionDeclaration, FUNCTION_DECLARATION_PROPERTIES)

#define GENERATE_FIELDS(TYPE, NAME) TYPE m_##NAME;

#define EXPAND_INIT_PARAMETERS(TYPE, NAME) const TYPE& NAME,
#define EXPAND_INIT_LIST(TYPE, NAME) m_##NAME(NAME),

using ExpressionPtr = SharedPtr<ASTNode>;

#define DEFINE_EXPRESSIONS(NAME, PROPERTIES)                                 \
    struct NAME : public ASTNode {                                           \
        explicit NAME(PROPERTIES(EXPAND_INIT_PARAMETERS) bool dummy = false) \
            : PROPERTIES(EXPAND_INIT_LIST) m_Dummy(dummy) {}                 \
                                                                             \
        PROPERTIES(GENERATE_FIELDS)                                          \
        bool m_Dummy = false;                                                \
    };

EXPRESSION_LIST(DEFINE_EXPRESSIONS);

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

    ExpressionPtr ParseVariableDeclaration() {
        if (Consume(TokenType::LET) && Consume(TokenType::EQUALS) && Consume(TokenType::IDENTIFIER)) {
            const Token& identToken = GetPrevToken();
            String ident = std::get<String>(identToken.data);
            // @TODO(xearty): do something with the identifier
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
        return ParseFunctionDeclaration()
            ?: nullptr;
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

ExpressionPtr Parse(const TokenCollection& tokens) {
    return Parser::Parse(tokens);
}

int main() {
    Lexer lexer("./program.zix");

    std::cout << "Program:\n";
    std::cout << lexer.GetStream() << std::endl;

    std::cout << "Tokens:\n";
    auto tokens = Tokenize(lexer).expect("Could not tokenize program");
    PrintTokens(tokens);

    ExpressionPtr astRoot = Parse(tokens);
}

