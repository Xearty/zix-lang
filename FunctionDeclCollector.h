#pragma once

#include "CommonTypes.h"
#include "ASTVisitor.h"
#include "ASTNode.h"

struct FunctionDeclMetaData {
    Vector<FuncParam> parameters;
    String returnType;
};

struct FunctionDeclCollector final : public ASTVisitor {
    virtual void Visit(const TopStatements& node) override {
        for (const auto& stat : node.GetStatements()) {
            stat->Accept(*this);
        }
    }

    virtual void Visit(const FunctionDeclaration& decl) override {
        FunctionDeclMetaData meta;
        meta.parameters = decl.GetParameters();
        meta.returnType = decl.GetReturnType();
        m_FunctionDecls.emplace(decl.GetName(), meta);
        decl.GetBody()->Accept(*this);
    }

    void DumpDeclarations(std::ostream& out = std::cout) {
        for (const auto& [name, meta] : m_FunctionDecls) {
            out << "fn " << name << "(";
            if (!meta.parameters.empty()) {
                out << meta.parameters[0].name << ": " << meta.parameters[0].type;
                for (size_t i = 1; i < meta.parameters.size(); ++i) {
                    out << ", " << meta.parameters[i].name << ": " << meta.parameters[i].type;
                }
            }
            out << ") -> " << meta.returnType << std::endl;
        }
    }

    HashMap<String, FunctionDeclMetaData> m_FunctionDecls;
};

