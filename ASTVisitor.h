#pragma once

#include "ASTNodeForwardDeclare.h"

#define DEFINE_VISITOR_INTERFACE(NAME, PROPERTIES) \
    virtual void Visit(const NAME& node) {}

struct ASTVisitor {
    AST_NODES_LIST(DEFINE_VISITOR_INTERFACE)
};

