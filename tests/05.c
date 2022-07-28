int delay() {
    char i;
    char j;
    i = 0;
    while (i < 250) {
        j = 0;
        while (j < 250) {
            j += 1;
        }
        i += 1;
    }
    return 0;
}

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
        *porta = ~(1 << (c & 7));
        c += 1;
        delay();
    }
    return 0;
}
