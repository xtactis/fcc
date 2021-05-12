#include <time.h>

#include "utils/common.h"

#include "C/parser.h"
#include "C/token.h"
#include "C/ir_gen.h"

#include "IR/IR.h"

char *codefile = NULL;

void printAST(Node *root, u64 indent, const Scope *current_scope) {
    if (root == NULL) return;
    for (u64 i = 0; i < indent; ++i) {
        if (i % 3 == 0) {
            putchar('|');
        } else {
            putchar(' ');
        }
    }
    if (root->scope != NULL) {
        current_scope = root->scope;
    }
    {
        char s[100];
        printf("%s\n", Token_toStr(s, *root->token));
    }
    if (root->token->type == '?' || root->token->type == TOKEN_FOR || root->token->type == TOKEN_FOR_COND || root->token->type == TOKEN_IF || root->token->type == TOKEN_WHILE || root->token->type == TOKEN_DO) {
        printAST(root->cond, indent+3, current_scope);
    }
    if (root->token->type == TOKEN_DECLARATION) {
        SymbolTableEntry *entry = root->token->entry;
        assert(entry != NULL);
        if (entry->type->is_function) {
            printAST(entry->type->function_type->block, indent+3, current_scope);
        }
    }
    printAST(root->left, indent+3, current_scope);
    printAST(root->right, indent+3, current_scope);
}

String read_file(char *filename) {
    FILE *fp = fopen(filename, "rb");
    String file_data;
    fseek(fp, 0L, SEEK_END);
    file_data.count = ftell(fp);
    rewind(fp);
    file_data.data = malloc((file_data.count+1) * sizeof(char));
    fread(file_data.data, sizeof(char), file_data.count, fp);
    fclose(fp);
    file_data.data[file_data.count] = 0;
    return file_data;
}

void parse_args(int argc, char **argv) {
    for (u64 i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            ++i;
            // get output file
        } else {
            codefile = argv[i];;
        }
    }
    if (codefile == NULL) {
        error(0, "Error: No file!");
    }
}

// TODO(mdizdar): make it actually read from a file instead of this stupid hardcoded bs
int main(int argc, char **argv) {
    parse_args(argc, argv);
    String code = read_file(codefile);
    puts(CYAN "***CODE***" RESET);
    puts(code.data);
    SymbolTable st;
    SymbolTable_init(&st, 10);
    
    Parser parser = (Parser){
        .lexer = {
            .code = code,
            .token_at = calloc(code.count+5, sizeof(CachedToken)),
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
    
    Node *AST = Parser_parse(&parser);
    
    DynArray generated_IR;
    generated_IR.element_size = sizeof(IR*);
    generated_IR.capacity = 0;
    {
        IR *main_call = malloc(sizeof(IR));
        main_call->instruction = OP_CALL;
        main_call->operands[0].type = OT_LABEL;
        main_call->operands[0].named = true;
        main_call->operands[0].label_name.data = "main";
        main_call->operands[0].label_name.count = 5;
        DynArray_add(&generated_IR, &main_call);
    }
    puts(CYAN "***AST***" RESET);
    printAST(AST, 0, st.scope);
    
    puts(CYAN "****IR****" RESET);
    LoopContext loop_context;
    loop_context.in_loop = false;
    IR_generate(AST, &generated_IR, st.scope, &loop_context);
    IR_print(generated_IR.data, generated_IR.count);
    
    /*
puts(CYAN "**TOKENS**" RESET);
    Lexer lexer = (Lexer){
        .code = code,
        .token_at = calloc(code.count+5, sizeof(CachedToken)),
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
    
    puts(CYAN "\n***AST***" RESET);
    
    SymbolTable st;
    SymbolTable_init(&st, 10);
    
    clock_t begin = clock();
    
    Parser parser = (Parser){
        .lexer = {
            .code = code,
            .token_at = calloc(code.count+5, sizeof(CachedToken)),
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
    
    Node *AST = Parser_parse(&parser);
    
    clock_t end = clock();
    
    printAST(AST, 0);
    
    puts(CYAN "\n***SYMBOL TABLE***" RESET);
    
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
    
    puts(CYAN "\n***STATS***" RESET);
    
    printf("Lines of code: %llu\n", parser.lexer.cur_line);
    printf("Time: ~%lf seconds\n", (double)(end-begin) / CLOCKS_PER_SEC);
    printf("Memory: %llu bytes (%.2lf MB)\n", parser.arena->total_capacity, 1.*parser.arena->total_capacity/1024/1024);
    */
    return 0;
}
