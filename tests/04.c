int main() {
    char *ddra;
    char *porta;
    ddra = 58;
    porta = 59;
    *ddra = 255;
    *porta = 247;
    if (1) {
        *porta = ~4;
    }
    return 0;
}
