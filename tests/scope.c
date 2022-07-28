#include <stdio.h>

int main() {
    {
        int a = 3;
        printf("%d %p\n", a, &a);
    }
    {
        int b;
        printf("%d %p\n", b, &b);
    }
    return 0;
}