#include <time.h>

#include "utils/common.h"

#include "C/token.h"
#include "C/parser.h"
void printAST(Node *root, u64 indent) {
    if (root == NULL) return;
    for (u64 i = 0; i < indent; ++i) {
        if (i % 3 == 0) {
            putchar('|');
        } else {
            putchar(' ');
        }
    }
    char s[100];
    printf("%s\n", Token_toStr(s, *root->token));
    if (root->token->type == '?' || root->token->type == TOKEN_FOR || root->token->type == TOKEN_FOR_COND || root->token->type == TOKEN_IF || root->token->type == TOKEN_WHILE || root->token->type == TOKEN_DO) {
        printAST(root->cond, indent+3);
    }
    printAST(root->left, indent+3);
    printAST(root->right, indent+3);
}

// TODO(mdizdar): make it actually read from a file instead of this stupid hardcoded bs
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
        "  char c = '\\n'                       \n"
        "  const char * str = \"sdf fdfs sdsf\";\n"
        "  return 42 + str[6] / c;              \n"
        "}                                      \n";
    puts(code);
    puts(CYAN "**TOKENS**" RESET);
    
    Lexer lexer = (Lexer){
        .code = { .data = code, .count = strlen(code) },
        .token_at = calloc(strlen(code)+5, sizeof(CachedToken)),
        .token_arena = Arena_init(4096),
        .pos = 0,
        .peek = 0,
        .cur_line = 1,
#pragma warning(suppress: 4221) 
    };
    // NOTE(mdizdar): this is just so it doesn't scream about initializing .symbol_table with the address of a variable that's on the stack
    Token *t;
    char s[1000];
    do {
        t = Lexer_peekNextToken(&lexer);
        printf("%s\n", Token_toStr_long(s, *t));
    } while (t->type != TOKEN_ERROR);
    puts("");
    
    puts(CYAN "***EXPR***" RESET);
    // NOTE(mdizdar): make sure to add an extra new line at the end of the file or sth
    char *expr;
    expr = 
        "struct foo {\n"
        "  signed const long * const volatile const x;\n"
        "  struct {\n"
        "    int y;\n"
        "  } w;\n"
        "  unsigned char *px;\n"
        "} z;\n"
        "int zz;\n"
        "for (zz = 0; zz < 5; ++z) {\n"
        "  int tmp = z.x;\n"
        "  z.x = z.w.y;\n"
        "  z.w.y = tmp;\n"
        "}\n"
        "z.x = 2+3*5%6*(1/4+3);\n"
        "zz = z.x += 2+3*(4-5)%(y?6+7:7*8);\n";
    "c1?++t1--:c2?t2++:f;\n";
    "&*p++.;\n";
    "foo(a, b, c, d)[2][3];\n";
    ";;;\n";
    "if (x == y) {\n  for (i = 0; i < n; ++i)\n    printf(\"%d\", i);\n}\n";
    puts(expr);
    
    SymbolTable st;
    SymbolTable_init(&st, 10);
    
    Parser parser = (Parser){
        .lexer = {
            .code = { .data = expr, .count = strlen(expr) },
            .token_at = calloc(strlen(code)+5, sizeof(CachedToken)),
            .token_arena = Arena_init(4096),
            .pos = 0,
            .peek = 0,
            .cur_line = 1,
        },
        .symbol_table = &st,
        .arena = Arena_init(4096),
        .type_arena = Arena_init(4096)
#pragma warning(suppress: 4221) 
    };
    // NOTE(mdizdar): this is just so it doesn't scream about initializing .symbol_table with the address of a variable that's on the stack
    
    printAST(Parser_parse(&parser), 0);
    
    for (u64 i = 0; i < parser.symbol_table->scope->capacity; ++i) {
        printf("%llu: ", i);
        if (parser.symbol_table->scope->hash_table[i].name.count == 0) { printf("\n"); continue; }
        SymbolTableEntry *entry = &parser.symbol_table->scope->hash_table[i];
        if (entry->type) {
            printf("{ name: %s; type: %d; def_line: %llu }\n", entry->name.data, entry->type->basic_type, entry->definition_line);
        } else {
            printf("{ name: %s; type: UNKNOWN; def_line: %llu }\n", entry->name.data, entry->definition_line);
        }
    }
    
    
    /*
    u64 N = 1000000, len = strlen(expr);
    char *long_code = malloc(N*len+25);
    for (u64 i = 0; i < N; ++i) {
        strcpy(long_code+i*len, expr);
    }
    long_code[N*len] = 0;
    
    parser = (Parser){
        .lexer = {
            .symbol_table = &st, // doesn't use this yet though
            .code = { .data = long_code, .count = N*len },
            .token_at = calloc(N*len+5, sizeof(CachedToken)),
            .token_arena = Arena_init(4096),
            .pos = 0,
            .peek = 0,
            .cur_line = 1,
        },
        .arena = Arena_init(4096),
        .type_arena = Arena_init(4096)
#pragma warning(suppress: 4221) 
    };
    // NOTE(mdizdar): this is just so it doesn't scream about initializing .symbol_table with the address of a variable that's on the stack
    
    clock_t begin = clock();
    
    Parser_parse(&parser);
    
    clock_t end = clock();
    
    printf("~Time taken for %llu expr: %lf\n", N, (double)(end-begin) / CLOCKS_PER_SEC);
    printf("Memory: %llu\n", parser.arena->total_capacity);
    */
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
