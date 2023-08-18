#include <iostream>
#include "Result.h"
#include "Lexer.h"
#include "Utils.h"

int main() {
    Lexer lexer("./program.zix");

    std::cout << "Program:\n";
    std::cout << lexer.GetStream() << std::endl;

    std::cout << "Tokens:\n";
    auto tokens = Tokenize(lexer).expect("Could not tokenize program");
    PrintTokens(tokens);
}

