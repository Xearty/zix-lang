#pragma once

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

