#include "utils/common.h"

#include "C/token.h"
#include "C/parser.h"

int main(int argc, char **argv) {
    if (false) {
        if (argc == 1) {
            error("No arguments provided");
        }
    }
    puts(CYAN "***CODE***" RESET);
    char *code =
        "int main() {                           \n"
        "  const char * str = \"sdf fdfs sdsf\";\n"
        "  return 42 + str[6];                  \n"
        "}                                      \n";
    puts(code);
    puts(CYAN "**TOKENS**" RESET);
    Lexer lexer;
    SymbolTable st;
    lexer.symbol_table = &st;
    lexer.code.data = code;
    lexer.code.count = strlen(code);
    lexer.pos = 0;
    Token t;
    do {
        t = getNextToken(&lexer);
        Token_print(t);
    } while (t.type != TOKEN_ERROR);
    /*
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
    }*/
    return 0;
}
