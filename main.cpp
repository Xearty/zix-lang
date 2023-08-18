#include <iostream>
#include <memory>
#include "Result.h"
#include "Lexer.h"
#include "Utils.h"

struct ASTNode {};

struct FuncParam {
    std::string name;
    std::string type;
};

#define NO_PROPERTIES(MACRO)

#define TOP_STATEMENTS_PROPERTIES(MACRO) \
    MACRO(std::vector<ExpressionPtr>, Statements)

#define FOR_EXPRESSION_PROPERTIES(MACRO) \
    MACRO(ExpressionPtr, Initialization)      \
    MACRO(ExpressionPtr, Condition)           \
    MACRO(ExpressionPtr, Increment)

#define FUNCTION_DECLARATION_PROPERTIES(MACRO) \
    MACRO(std::string, Name)                   \
    MACRO(std::vector<FuncParam>, Parameters)  \
    MACRO(std::string, ReturnType)             \
    MACRO(ExpressionPtr, Body)

#define EXPRESSION_LIST(MACRO)                                  \
    MACRO(TopStatements, TOP_STATEMENTS_PROPERTIES)             \
    MACRO(ForStatement, FOR_EXPRESSION_PROPERTIES)              \
    MACRO(FunctionDeclaration, FUNCTION_DECLARATION_PROPERTIES)

#define GENERATE_FIELDS(TYPE, NAME) TYPE m_##NAME;

#define EXPAND_INIT_PARAMETERS(TYPE, NAME) const TYPE& NAME,
#define EXPAND_INIT_LIST(TYPE, NAME) m_##NAME(NAME),

using ExpressionPtr = std::shared_ptr<ASTNode>;

#define DEFINE_EXPRESSIONS(NAME, PROPERTIES)                                 \
    struct NAME : public ASTNode {                                           \
        explicit NAME(PROPERTIES(EXPAND_INIT_PARAMETERS) bool dummy = false) \
            : PROPERTIES(EXPAND_INIT_LIST) m_Dummy(dummy) {}                 \
                                                                             \
        PROPERTIES(GENERATE_FIELDS)                                          \
        bool m_Dummy = false;                                                \
    };

EXPRESSION_LIST(DEFINE_EXPRESSIONS);

#define LIST_EXPRESSIONS(NAME, PROPERTIES) NAME,

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
            std::string ident = std::get<std::string>(identToken.data);
            // @TODO(xearty): do something with the identifier
        }

        return nullptr;
    }

    bool ParseParameter(std::vector<FuncParam>& params) {
        while (Consume(TokenType::IDENTIFIER)) {
            const Token& paramNameIdent = GetPrevToken();
            std::string paramName = std::get<std::string>(paramNameIdent.data);

            if (Consume(TokenType::COLON) && Consume(TokenType::IDENTIFIER)) {
                const Token& paramTypeIdent = GetPrevToken();
                std::string paramType = std::get<std::string>(paramTypeIdent.data);
                params.push_back(FuncParam{ std::move(paramName), std::move(paramType) });
                return true;
            }
        }
        return false;
    }

    ExpressionPtr ParseFunctionDeclaration() {
        auto ParseParameterList = [&](std::vector<FuncParam>& params) -> bool {
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
            std::string functionIdent = std::get<std::string>(identToken.data);

            std::vector<FuncParam> params;
            if (ParseParameterList(params)) {
                if (Consume(TokenType::ARROW) && Consume(TokenType::IDENTIFIER)) {
                    const Token& returnTypeIdent = GetPrevToken();
                    std::string returnType = std::get<std::string>(returnTypeIdent.data);

                    if (auto body = ParseBody()) {
                        return std::make_shared<FunctionDeclaration>(functionIdent, params, returnType, body);
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
        std::vector<ExpressionPtr> statements;
        while (auto statement = ParseTopStatement()) {
            statements.push_back(std::move(statement));
        }
        return std::make_shared<TopStatements>(statements);
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

