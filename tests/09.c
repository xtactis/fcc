void delay() {
	char i;
	char j;
    char k;
	i = 0;
	while (i < 250) {
		i += 1;
		j = 0;
		while (j < 250) {
			j += 1;
            k = 0;
            while (k < 2) {
                k += 1;
            }
		}
	}
    return;
}

char fib(char n) {
    if (n < 2) return 1;
    return fib(n-1) + fib(n-2);
}

int main() {
	char *ddra;
	char *porta;
	ddra = 58;
    *ddra = 255;
    porta = 59;
    *porta = 247;
	char c;
	c = 0;
	while (1) {
		*porta = ~fib(c);
        if (c == 12) {
            c += 244;
        }
        c += 1;
        delay();
	}
	return 0;
}
