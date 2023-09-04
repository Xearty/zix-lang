#pragma once

#include "CommonTypes.h"
#include "ASTVisitor.h"
#include "ASTNodeDefinitions.h"
#include "Token.h"

struct ASTNode {
    virtual const char* GetNodeName() const = 0;

    virtual void Accept(ASTVisitor& visitor) = 0;
    virtual void Accept(ASTVisitor&& visitor) = 0;
};

struct FuncParam {
    String name;
    String type;
};

using ASTNodeRef = SharedPtr<ASTNode>;

#define GENERATE_FIELDS(TYPE, NAME) TYPE m_##NAME;

#define EXPAND_INIT_PARAMETERS(TYPE, NAME) const TYPE& NAME,
#define EXPAND_INIT_LIST(TYPE, NAME) m_##NAME(NAME),

#define DEFINE_PROPERTY_GETTERS(TYPE, NAME) \
    TYPE& Get##NAME() { return m_##NAME; }

#define DEFINE_PROPERTY_GETTERS_CONST(TYPE, NAME) \
    const TYPE& Get##NAME() const { return m_##NAME; }

#define DEFINE_AST_NODES(NAME, PROPERTIES)                                   \
    struct NAME final : public ASTNode {                                     \
        explicit NAME(PROPERTIES(EXPAND_INIT_PARAMETERS) bool dummy = false) \
            : PROPERTIES(EXPAND_INIT_LIST) m_Dummy(dummy) {}                 \
                                                                             \
        virtual void Accept(ASTVisitor& visitor) override {                  \
            visitor.Visit(*this);                                            \
        }                                                                    \
                                                                             \
        virtual void Accept(ASTVisitor&& visitor) override {                 \
            visitor.Visit(*this);                                            \
        }                                                                    \
                                                                             \
        virtual const char* GetNodeName() const override {                   \
            return #NAME;                                                    \
        }                                                                    \
                                                                             \
        PROPERTIES(DEFINE_PROPERTY_GETTERS);                                 \
        PROPERTIES(DEFINE_PROPERTY_GETTERS_CONST);                           \
        PROPERTIES(GENERATE_FIELDS);                                         \
        bool m_Dummy = false;                                                \
    };

AST_NODES_LIST(DEFINE_AST_NODES);

