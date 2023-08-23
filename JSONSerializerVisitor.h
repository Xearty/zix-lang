#pragma once

#include "ASTNode.h"
#include "ASTVisitor.h"
#include "Token.h"
#include <iostream>
#include <type_traits>

#define SERIALIZE_PROPERTIES(TYPE, NAME)             \
    ++currentProp;                                   \
    std::cout << '"' << #NAME << "\": ";             \
    Serialize(node.Get##NAME());                     \
    PrepareForNextElement(currentProp != totalProps);

#define COUNT_PROPERTIES(TYPE, NAME) ++totalProps;

#define DEFINE_VISITOR_OVERLOADS(NAME, PROPERTIES)  \
    virtual void Visit(const NAME& node) override { \
        int totalProps = 0;                         \
        int currentProp = 0;                        \
        PROPERTIES(COUNT_PROPERTIES);               \
        std::cout << "{\n";                         \
        PushIndentLevel();                          \
        Indent();                                   \
        std::cout << "\"NodeType\": \""#NAME"\"";   \
        if (totalProps > 0) {                       \
            PrepareForNextElement(true);            \
        }                                           \
        PROPERTIES(SERIALIZE_PROPERTIES);           \
        PopIndentLevel();                           \
        Indent();                                   \
        std::cout << "}";                           \
    }


class JSONSerializerVisitor final : public ASTVisitor {
public:
    AST_NODES_LIST(DEFINE_VISITOR_OVERLOADS);

private:
    void Serialize(const ASTNodeRef& expr) {
        expr->Accept(*this);
    }

    void Serialize(int val) {
        std::cout << '"' << val << '"';
    }

    void Serialize(const String& val) {
        std::cout << '"' << val << '"';
    }

    void Serialize(TokenType token) {
        std::cout << '"' << GetTokenName(token) << '"';
    }

    void Serialize(const Vector<ASTNodeRef>& vec) {
        SerializeVector(vec);
    }

    void Serialize(const FuncParam& param) {
        std::cout << "{\n";
        PushIndentLevel();
        Indent();
        std::cout << "\"Identifier\": ";
        Serialize(param.name);
        PrepareForNextElement(true);
        std::cout << "\"Type\": ";
        Serialize(param.type);
        std::cout << '\n';
        PopIndentLevel();
        Indent();
        std::cout << "}";
    }

    void Serialize(const Vector<FuncParam>& vec) {
        SerializeVector(vec);
    }

    template <typename T>
    void SerializeVector(const Vector<T>& vec) {
        std::cout << "[";
        if (!vec.empty()) {
            PushIndentLevel();
            std::cout << '\n';
            Indent();

            for (size_t i = 0; i < vec.size() - 1; ++i) {
                Serialize(vec[i]);
                PrepareForNextElement(true);
            }

            Serialize(vec.back());
            PopIndentLevel();
            std::cout << '\n';
            Indent();
        }
        std::cout << ']';
    }

    void PrepareForNextElement(bool shouldInsertComma) {
        if (shouldInsertComma) std::cout << ",";
        std::cout << "\n";
        if (shouldInsertComma) Indent();
    }

    void Indent() {
        for (int i = 0; i < m_IndentLevel; ++i) {
            std::cout << "    ";
        }
    }

    void PushIndentLevel() {
        ++m_IndentLevel;
    }

    void PopIndentLevel() {
        --m_IndentLevel;
    }

private:
    int m_IndentLevel = 0;
};

