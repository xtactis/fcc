#ifndef PRINTING_H
#define PRINTING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"
#define RESET   "\x1B[0m"
#define NOT_IMPL error("Not implemented!");

_Noreturn void error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    fputs(RED, stderr);
    vfprintf(stderr, fmt, args);
    fputs(RESET "\n", stderr);
    
    va_end(args);
    exit(1);
}

void warning(const char *s) {
    printf(YELLOW "WARNING: %s\n" RESET, s);
}

#endif // PRINTING_H