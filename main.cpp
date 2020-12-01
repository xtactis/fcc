#include <cstdio>

#include <vector>
#include <string>

#include "utils/common.hpp"

#include "C/token.hpp"
#include "C/parser.hpp"

int main(int argc, char **argv) {
    if (false) {
        if (argc == 1) {
            error("No arguments provided");
        }
    }
    puts(CYAN "***CODE***" RESET);
    const std::string code =
        "int main() {\n"
        "  return 42;\n"
        "}             ";
    puts(code.c_str());
    puts("");
    puts(CYAN "**TOKENS**" RESET);
    std::vector<Token> tokens = CParser::tokenize(code);
    Token::printTokens(tokens);
    puts(CYAN "****AST***" RESET);
    AST ast;
    parse(tokens, ast);
    puts("");
    validate(ast);
    puts(CYAN "****IR*****" RESET);
    const std::vector<IR> ir = generateIR(ast);
    for (const auto &e: ir) {
        e.print();
    }
}
