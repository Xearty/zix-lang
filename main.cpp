#include <iostream>
#include <memory>
#include "Result.h"
#include "Lexer.h"
#include "Utils.h"
#include "ASTNode.h"
#include "Parser.h"
#include "JSONSerializerVisitor.h"

#include "CommonTypes.h"

int main() {
    Lexer lexer("./program.zix");

    std::cout << "Program:\n";
    std::cout << lexer.GetStream() << std::endl;

    std::cout << "Tokens:\n";
    auto tokens = Tokenize(lexer).expect("Could not tokenize program");
    PrintTokens(tokens);

    ExpressionPtr astRoot = Parse(tokens);
    std::cout << std::endl;
    astRoot->Accept(JSONSerializerVisitor{});
}

