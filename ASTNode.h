#pragma once

#include "CommonTypes.h"

struct FuncParam {
    String name;
    String type;
};

struct ASTNode {};

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

#define VARIABLE_DECLARATION_PROPERTIES(MACRO) \
    MACRO(String, Name)                        \
    MACRO(ExpressionPtr, InitialValue)

#define INTEGER_LITERAL_EXPRESSION_PROPERTIES(MACRO) \
    MACRO(int, Value)

#define IDENTIFIER_EXPRESSION_PROPERTIES(MACRO) \
    MACRO(String, Name)

#define EXPRESSION_LIST(MACRO)                                             \
    MACRO(TopStatements, TOP_STATEMENTS_PROPERTIES)                        \
    MACRO(ForStatement, FOR_EXPRESSION_PROPERTIES)                         \
    MACRO(FunctionDeclaration, FUNCTION_DECLARATION_PROPERTIES)            \
    MACRO(VariableDeclaration, VARIABLE_DECLARATION_PROPERTIES)            \
    MACRO(IntegerLiteralExpression, INTEGER_LITERAL_EXPRESSION_PROPERTIES) \
    MACRO(IdentifierExpression, IDENTIFIER_EXPRESSION_PROPERTIES)

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

