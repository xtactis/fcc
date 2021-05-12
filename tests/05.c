int foo() {
    int x;
    int y;
    x = 5;
    y = ~x + 3;
    return x * y;
}

int main() {
    foo();
    
    int i;
    i = 0;
    while (i < 10) {
        i++;
    }
    for (i = 42; i < 69; i += 3) {
        --i;
        if (i == 50) {
            break;
        } else if (i == 51) continue;
    }
    return 0;
}
