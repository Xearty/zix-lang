#pragma once

#define NO_PROPERTIES(MACRO)

#define TOP_STATEMENTS_PROPERTIES(MACRO) \
    MACRO(Vector<ASTNodeRef>, Statements)

#define FOR_STATEMENT_PROPERTIES(MACRO) \
    MACRO(ASTNodeRef, Initialization) \
    MACRO(ASTNodeRef, Condition)      \
    MACRO(ASTNodeRef, Increment)

#define FUNCTION_DECLARATION_PROPERTIES(MACRO) \
    MACRO(String, Name)                        \
    MACRO(Vector<FuncParam>, Parameters)       \
    MACRO(String, ReturnType)                  \
    MACRO(ASTNodeRef, Body)

#define VARIABLE_DECLARATION_PROPERTIES(MACRO) \
    MACRO(String, Name)                        \
    MACRO(ASTNodeRef, InitialValue)

#define INTEGER_LITERAL_EXPRESSION_PROPERTIES(MACRO) \
    MACRO(int, Value)

#define IDENTIFIER_EXPRESSION_PROPERTIES(MACRO) \
    MACRO(String, Name)

#define AST_NODES_LIST(MACRO)                                              \
    MACRO(TopStatements, TOP_STATEMENTS_PROPERTIES)                        \
    MACRO(ForStatement, FOR_STATEMENT_PROPERTIES)                          \
    MACRO(FunctionDeclaration, FUNCTION_DECLARATION_PROPERTIES)            \
    MACRO(VariableDeclaration, VARIABLE_DECLARATION_PROPERTIES)            \
    MACRO(IntegerLiteralExpression, INTEGER_LITERAL_EXPRESSION_PROPERTIES) \
    MACRO(IdentifierExpression, IDENTIFIER_EXPRESSION_PROPERTIES)

