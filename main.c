#include <time.h>

#include "utils/common.h"

#include "C/parser.h"
#include "C/token.h"
#include "C/type_check.h"
#include "C/ir_gen.h"

#include "IR/IR.h"
#include "AVR/AVR.h"
#include "IR2AVR/IR2AVR.h"

char *codefile = NULL;
char *outfile = NULL;
bool silent = false;

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
        char s[256];
        printf("%s", Token_toStr(s, *root->token));
        if (root->token->type == TOKEN_DECLARATION) {
            printf(" %s", SymbolTableEntry_toStr(s, root->token->entry));
        }
        puts("");
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

void saveAST_labels(Node *AST, const Scope *current_scope, FILE *fp, u64 id) {
    if (AST == NULL) return;
    if (AST->scope != NULL) {
        current_scope = AST->scope;
    }
    if (AST->token->type != TOKEN_NEXT && AST->token->type != TOKEN_DECLARATION) {
        // NOTE(mdizdar): this is here so it doesn't generate unnecessary nodes
        char s[100];
        fprintf(fp, "%lu[label=\"%s\"]\n", id, Token_toStr(s, *AST->token));
    }
    if (AST->token->type == '?' || AST->token->type == TOKEN_FOR || AST->token->type == TOKEN_FOR_COND || AST->token->type == TOKEN_IF || AST->token->type == TOKEN_WHILE || AST->token->type == TOKEN_DO) {
        saveAST_labels(AST->cond, current_scope, fp, id+1024);
        saveAST_labels(AST->left, current_scope, fp, 2*id+1);
        saveAST_labels(AST->right, current_scope, fp, 2*id+2);
    } else if (AST->token->type == TOKEN_DECLARATION) {
        SymbolTableEntry *entry = AST->token->entry;
        assert(entry != NULL);
        if (entry->type->is_function) {
            char s[100];
            fprintf(fp, "%lu[label=\"%s\"]\n", id, Token_toStr(s, *AST->token));
            saveAST_labels(entry->type->function_type->block, current_scope, fp, 2*id+1);
        }
    } else {
        saveAST_labels(AST->left, current_scope, fp, 2*id+1);
        saveAST_labels(AST->right, current_scope, fp, 2*id+2);
    }
}

void saveAST_edges(Node *AST, const Scope *current_scope, FILE *fp, u64 id, u64 prev) {
    if (AST == NULL) return;
    if (AST->scope != NULL) {
        current_scope = AST->scope;
    }
    if (AST->token->type == '?' || AST->token->type == TOKEN_FOR || AST->token->type == TOKEN_FOR_COND || AST->token->type == TOKEN_IF || AST->token->type == TOKEN_WHILE || AST->token->type == TOKEN_DO) {
        fprintf(fp, "%lu->%lu\n", prev, id);
        saveAST_edges(AST->left, current_scope, fp, 2*id+1, id);
        saveAST_edges(AST->cond, current_scope, fp, id+1024, id);
        saveAST_edges(AST->right, current_scope, fp, 2*id+2, id);
    } else if (AST->token->type == TOKEN_NEXT) {
        saveAST_edges(AST->left, current_scope, fp, 2*id+1, prev);
        saveAST_edges(AST->right, current_scope, fp, 2*id+2, prev);
    } else if (AST->token->type == TOKEN_DECLARATION) {
        SymbolTableEntry *entry = AST->token->entry;
        assert(entry != NULL);
        if (entry->type->is_function) {
            fprintf(fp, "%lu->%lu\n", prev, id);
            saveAST_edges(entry->type->function_type->block, current_scope, fp, 2*id+1, id);
        }
    } else {
        fprintf(fp, "%lu->%lu\n", prev, id);
        saveAST_edges(AST->left, current_scope, fp, 2*id+1, id);
        saveAST_edges(AST->right, current_scope, fp, 2*id+2, id);
    }
}

void saveAST(Node *AST, const Scope *current_scope, char *filename) {
    u64 len = strlen(filename);
    char *of = malloc(sizeof(char) * len+15);
    strcpy(of, filename);
    strcat(of, "_AST.dot");
    FILE *fp = fopen(of, "w");
    fprintf(fp, "digraph G {\n");
    fprintf(fp, "0[label=\"Program\"]\n");
    saveAST_labels(AST, current_scope, fp, 1);
    saveAST_edges(AST, current_scope, fp, 1, 0);
    fprintf(fp, "}");
    fclose(fp);
}

void saveCFG(IRArray *ir, char *filename) {
    u64 len = strlen(filename);
    char *of = malloc(sizeof(char) * len+15);
    strcpy(of, filename);
    strcat(of, "_CFG.dot");
    FILE *fp = fopen(of, "w");
    fprintf(fp, "digraph G {\nnode [shape=box, fontname=Courier];\n");
    
    IR *irs = ir->data;
    for (u64 i = 0; i < ir->count; ++i) {
        fprintf(fp, "%lu[label=\"", irs[i].block->id);
        u64 end = irs[i].block->end;
        for (; i <= end; ++i) {
            IR_saveOne(&irs[i], fp, "\\l");
        }
        --i;
        fprintf(fp, "\"]\n");
    }
    
    for (u64 i = 0; i < ir->count; ++i) {
        if (irs[i].block->next) fprintf(fp, "%lu->%lu\n", irs[i].block->id, irs[i].block->next->id);
        if (irs[i].block->jump) fprintf(fp, "%lu->%lu\n", irs[i].block->id, irs[i].block->jump->id);
        u64 end = irs[i].block->end;
        for (; i < end; ++i);
    }
    
    fprintf(fp, "}");
    fclose(fp);
}

String read_file(char *filename) {
    FILE *fp = fopen(filename, "rb");
    String file_data;
    fseek(fp, 0L, SEEK_END);
    file_data.count = ftell(fp);
    rewind(fp);
    file_data.data = malloc((file_data.count+1) * sizeof(char));
    size_t actually_read = fread(file_data.data, sizeof(char), file_data.count, fp);
    if (actually_read != file_data.count) {
        error(-1, "Error reading file.");
    }
    fclose(fp);
    file_data.data[file_data.count] = 0;
    return file_data;
}

void parse_args(int argc, char **argv) {
    for (s64 i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            ++i;
            outfile = argv[i];
        } else if (strcmp(argv[i], "-s") == 0) {
            silent = true;
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
    if (!silent) puts(CYAN "***CODE***" RESET);
    if (!silent) puts(code.data);
    SymbolTable st;
    SymbolTable_init(&st);
    
    Parser parser = (Parser){
        .lexer = {
            .code = code,
            .token_at = calloc(code.count+5, sizeof(CachedToken)),
            .token_arena = Arena_init(4096),
            .pos = 0,
            .peek = 0,
            .cur_line = 1,
            .cur_col = 0
        },
        .symbol_table = &st,
        .arena = Arena_init(4096),
        .type_arena = Arena_init(4096)
    };
    
    Node *AST = Parser_parse(&parser);
    
    IRArray generated_IR;
    IRArray_construct(&generated_IR);
    {
        IR *label = malloc(sizeof(IR));
        label->block = NULL;
        label->instruction = OP_LABEL;
        label->operands[0].type = OT_LABEL;
        label->operands[0].named = true;
        label->operands[0].label_name.data = "__start";
        label->operands[0].label_name.count = 8; 
        IRArray_push_ptr(&generated_IR, label);
        IR *main_call = malloc(sizeof(IR));
        main_call->block = NULL;
        main_call->instruction = OP_CALL;
        main_call->operands[0].type = OT_LABEL;
        main_call->operands[0].named = true;
        main_call->operands[0].label_name.data = "main";
        main_call->operands[0].label_name.count = 5;
        IRArray_push_ptr(&generated_IR, main_call);
        IR *jump = malloc(sizeof(IR));
        jump->block = NULL;
        jump->instruction = OP_JUMP;
        jump->operands[0].type = OT_LABEL;
        jump->operands[0].named = true;
        jump->operands[0].label_name.data = "__start";
        jump->operands[0].label_name.count = 8; 
        IRArray_push_ptr(&generated_IR, jump);
    }
    if (!silent) puts(CYAN "***AST***" RESET);
    if (!silent) printAST(AST, 0, st.scope);
    if (outfile) saveAST(AST, st.scope, outfile);
    
    if (!silent) puts(CYAN "****IR****" RESET);
    type_check(AST, NULL);
    IRContext context = {.global = true};
    context.in_loop = false;
    IR_generate(AST, &generated_IR, st.scope, &context);
    if (!silent) IR_print(generated_IR.data, generated_IR.count);

    LabelArray labels = findLabels(&generated_IR);

    generated_IR = IR_resolve_phi(&generated_IR, &labels);
    if (!silent) puts(CYAN "***Phi resolved IR***" RESET);
    if (!silent) IR_print(generated_IR.data, generated_IR.count);
    
    if (!silent) puts(CYAN "***AVR***" RESET);
    AVRArray generated_AVR;
    AVRArray_construct(&generated_AVR);

    IR2AVR(&generated_IR, &generated_AVR, &labels, temporary_index);
    if (!silent) printAVR(&generated_AVR);
    
    if (!silent) puts(CYAN "***HEX***" RESET);
    if (!silent) printIntelHex(&generated_AVR);
    
    if (outfile) {
        IR_save(&generated_IR, outfile);
        saveCFG(&generated_IR, outfile);
        saveAVR(&generated_AVR, outfile);
        saveIntelHex(&generated_AVR, outfile);
    }
    
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
