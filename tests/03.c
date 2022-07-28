char foo() {
    return 247;
}

int main() {
    char *ddra;
    char *porta;
    ddra = 58;
    porta = 59;
    *ddra = 255;
    *porta = foo();
    return 0;
}
