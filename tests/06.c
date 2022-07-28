int main() {
    char *ddra;
    char *porta;
    char c;
    ddra = 58;
    porta = 59;
    *ddra = 255;
    *porta = 255;
    
    c = 0;
    while (1) {
        *porta = ~c;
        c += 1;
    }
    return 0;
}
