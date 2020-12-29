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
    SymbolTable st = {.resize_threshold = 0.7};
    lexer.symbol_table = &st;
    SymbolTable_init(lexer.symbol_table, 10);
    lexer.code.data = code;
    lexer.code.count = strlen(code);
    lexer.pos = 0;
    lexer.cur_line = 1;
    Token t;
    do {
        t = getNextToken(&lexer);
        Token_print(t);
    } while (t.type != TOKEN_ERROR);
    puts("");
    for (u64 i = 0; i < lexer.symbol_table->capacity; ++i) {
        printf("%llu: ", i);
        if (lexer.symbol_table->hash_table[i].name.count == 0) { printf("\n"); continue; }
        SymbolTableEntry *entry = &lexer.symbol_table->hash_table[i];
        printf("{ name: %s; type: %llu; def_line: %llu }\n", entry->name.data, entry->type, entry->definition_line);
    }
    /*
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
