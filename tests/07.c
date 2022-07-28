int delay() {
    char i;
    char j;
    for (i = 0; i < 250; i += 1) {
        for (j = 0; j < 250; j += 1);
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
    
    for (c = 0; 1; ++c) {
        *porta = ~(1 << (c & 7));
        delay();
    }
    return 0;
}
