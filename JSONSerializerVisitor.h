#pragma once

#include "ASTNode.h"
#include "ASTVisitor.h"
#include "Token.h"
#include <iostream>
#include <type_traits>

#define SERIALIZE_PROPERTIES(TYPE, NAME)             \
    ++currentProp;                                   \
    m_Output << '"' << #NAME << "\": ";             \
    Serialize(node.Get##NAME());                     \
    PrepareForNextElement(currentProp != totalProps);

#define COUNT_PROPERTIES(TYPE, NAME) ++totalProps;

#define DEFINE_VISITOR_OVERLOADS(NAME, PROPERTIES)  \
    virtual void Visit(const NAME& node) override { \
        int totalProps = 0;                         \
        int currentProp = 0;                        \
        PROPERTIES(COUNT_PROPERTIES);               \
        m_Output << "{\n";                         \
        PushIndentLevel();                          \
        Indent();                                   \
        m_Output << "\"NodeType\": \""#NAME"\"";   \
        if (totalProps > 0) {                       \
            PrepareForNextElement(true);            \
        }                                           \
        PROPERTIES(SERIALIZE_PROPERTIES);           \
        PopIndentLevel();                           \
        Indent();                                   \
        m_Output << "}";                           \
    }


class JSONSerializerVisitor final : public ASTVisitor {
public:
    explicit JSONSerializerVisitor(std::ostream& output = std::cout)
        : m_Output(output) {}

    AST_NODES_LIST(DEFINE_VISITOR_OVERLOADS);

private:
    void Serialize(const ASTNodeRef& expr) {
        expr->Accept(*this);
    }

    void Serialize(int val) {
        m_Output << '"' << val << '"';
    }

    void Serialize(const String& val) {
        m_Output << '"' << val << '"';
    }

    void Serialize(TokenType token) {
        m_Output << '"' << GetTokenName(token) << '"';
    }

    void Serialize(const Vector<ASTNodeRef>& vec) {
        SerializeVector(vec);
    }

    void Serialize(const FuncParam& param) {
        m_Output << "{\n";
        PushIndentLevel();
        Indent();
        m_Output << "\"Identifier\": ";
        Serialize(param.name);
        PrepareForNextElement(true);
        m_Output << "\"Type\": ";
        Serialize(param.type);
        m_Output << '\n';
        PopIndentLevel();
        Indent();
        m_Output << "}";
    }

    void Serialize(const Vector<FuncParam>& vec) {
        SerializeVector(vec);
    }

    template <typename T>
    void SerializeVector(const Vector<T>& vec) {
        m_Output << "[";
        if (!vec.empty()) {
            PushIndentLevel();
            m_Output << '\n';
            Indent();

            for (size_t i = 0; i < vec.size() - 1; ++i) {
                Serialize(vec[i]);
                PrepareForNextElement(true);
            }

            Serialize(vec.back());
            PopIndentLevel();
            m_Output << '\n';
            Indent();
        }
        m_Output << ']';
    }

    void PrepareForNextElement(bool shouldInsertComma) {
        if (shouldInsertComma) m_Output << ",";
        m_Output << "\n";
        if (shouldInsertComma) Indent();
    }

    void Indent() {
        for (int i = 0; i < m_IndentLevel; ++i) {
            m_Output << "    ";
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
    std::ostream& m_Output;
};

