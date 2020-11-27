#include <cstdio>
#include <cstdlib>

#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"
#define RESET   "\x1B[0m"
#define NOT_IMPL error("Not implemented!");

void error(const char* s) {
  fputs(RED "ERROR: ", stderr);
  fputs(s, stderr);
  fputs(RESET "\n", stderr);
  exit(0);
}

void warning(const char *s) {
  printf(YELLOW "WARNING: %s\n" RESET, s);
}
