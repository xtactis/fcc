char fib(char n) {
    if (n < 2) return 1;
    return fib(n-1) + fib(n-2);
}

int main() {
    char *ddra;
    char *porta;
    ddra = 58;
    porta = 59;
    *ddra = 255;
    *porta = ~fib(11);
    return 0;
}
