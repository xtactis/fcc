#include "printing.h"
#include <stdio.h>

_Noreturn void error(u64 lineno, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    fputs(RED, stderr);
    if (lineno > 0) {
        fprintf(stderr, "[Line %lu] ERROR ", lineno);
    }
    vfprintf(stderr, fmt, args);
    fputs(RESET "\n", stderr);
    
    va_end(args);
    exit(1);
}

_Noreturn void internal_error(const char * filename, u64 line_number) {
    fputs(RED, stderr);
    fprintf(stderr, "Internal compiler error at %s:%lu", filename, line_number);
    fputs(RESET "\n", stderr);
    exit(1);
}

void warning(u64 lineno, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    fputs(YELLOW, stderr);
    if (lineno > 0) {
        fprintf(stderr, "[Line %lu] WARNING ", lineno);
    }
    vfprintf(stderr, fmt, args);
    fputs(RESET "\n", stderr);
    
    va_end(args);
}
