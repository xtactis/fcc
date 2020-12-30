#include "utils/common.h"

#include "C/token.h"
#include "C/parser.h"


void printAST(Node *root, u64 indent) {
    for (u64 i = 0; i < indent; ++i) {
        if (i % 3 == 0) {
            putchar('|');
        } else {
            putchar(' ');
        }
    }
    char s[100];
    printf("%s\n", Token_toStr(s, root->token));
    if (root->token.type == '?') {
        printAST(root->cond, indent+3);
    }
    if (root->left != NULL) {
        printAST(root->left, indent+3);
    }
    if (root->right != NULL) {
        printAST(root->right, indent+3);
    }
}


int main(int argc, char **argv) {
    if (false) {
        if (argc == 1) {
            error(0, "No arguments provided");
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
        "  char c = '\\n'\n"
        "  const char * str = \"sdf fdfs sdsf\";\n"
        "  return 42 + str[6] / c;              \n"
        "}                                      \n";
    puts(code);
    puts(CYAN "**TOKENS**" RESET);
    
    SymbolTable st = {.resize_threshold = 0.7};
    SymbolTable_init(&st, 10);
    
    Lexer lexer = (Lexer){
        .symbol_table = &st, 
        .code = { .data = code, .count = strlen(code) },
        .pos = 0,
        .peek = 0,
        .cur_line = 1,
    };
    Token t;
    char s[1000];
    do {
        t = Lexer_peekNextToken(&lexer);
        printf("%s\n", Token_toStr_long(s, t));
    } while (t.type != TOKEN_ERROR);
    
    puts("");
    for (u64 i = 0; i < lexer.symbol_table->capacity; ++i) {
        printf("%llu: ", i);
        if (lexer.symbol_table->hash_table[i].name.count == 0) { printf("\n"); continue; }
        SymbolTableEntry *entry = &lexer.symbol_table->hash_table[i];
        printf("{ name: %s; type: %lld; def_line: %llu }\n", entry->name.data, entry->type, entry->definition_line);
    }
    
    puts(CYAN "***EXPR***" RESET);
    // NOTE(mdizdar): make sure to add an extra new line at the end of the file or sth
    char *expr;
    expr = "2+3*5%6*(1/4+3)\n";
    expr = "y += x += 2+3*(4-5)%(y?6+7:7*8)\n";
    puts(expr);
    
    Parser parser = (Parser){
        .lexer = {
            .symbol_table = &st, // doesn't use this yet though
            .code = { .data = expr, .count = strlen(expr) },
            .pos = 0,
            .peek = 0,
            .cur_line = 1,
        }
    };
    //Parser_parse(&parser);
    printAST(Parser_parse(&parser), 0);
    
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
