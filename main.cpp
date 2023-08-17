#include <iostream>
#include "Result.h"
#include "Lexer.h"
#include "Utils.h"

int main() {
    auto lexer = CreateLexer("./program.zix");
    defer(DestroyLexer(lexer));

    std::cout << "Program:\n";
    std::cout << lexer.stream << std::endl;

    std::cout << "Tokens:\n";
    auto tokens = Tokenize(lexer).expect("Could not tokenize program");
    PrintTokens(tokens);
}

