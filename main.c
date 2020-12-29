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
        "typedef struct {                       \n"
        "  int x;                               \n"
        "  union {                              \n"
        "    int int_val;                       \n"
        "    double real_val;                   \n"
        "  };                                   \n"
        "} Struct;                              \n"
        "                                       \n"
        "int main() {                           \n"
        "  const char * str = \"sdf fdfs sdsf\";\n"
        "  return 42 + str[6];                  \n"
        "}                                      \n";
    puts(code);
    puts(CYAN "**TOKENS**" RESET);
    Lexer lexer;
    SymbolTable st = {.resize_threshold = 0.7};
    SymbolTable_init(&st, 10);
    lexer = (Lexer){
        .symbol_table = &st, 
        .code = { .data = code, .count = strlen(code) },
        .pos = 0,
        .peek = 0,
        .cur_line = 1,
    };
    Token t;
    do {
        t = Lexer_peekNextToken(&lexer);
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
