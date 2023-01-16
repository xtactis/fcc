#ifndef PRINTING_H
#define PRINTING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "types.h"

#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"
#define RESET   "\x1B[0m"
#define NOT_IMPL error(0, "Not implemented!");

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

#endif // PRINTING_H