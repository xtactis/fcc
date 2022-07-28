void debounce() {
	char i;
	char j;
	i = 0;
	while (i < 250) {
		i += 1;
		j = 0;
		while (j < 250) {
			j += 1;
		}
	}
    return;
}

int main() {
	char *ddra;
	char *porta;
	char *ddrb;
	char *portb;
	char *pinb;
	ddra = 58;
    *ddra = 255;
	ddrb = 55;
    *ddrb = 0;
    portb = 56;
	*portb = 1;
    porta = 59;
    *porta = 247;
	pinb = 54;
	char c;
	c = 0;
	while (1) {
		if (!(*pinb & 1)) {
			c += 1;
            debounce();
		}
		*porta = ~c;
	}
	return 0;
}
