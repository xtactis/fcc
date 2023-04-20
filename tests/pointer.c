int main() {
    int x;
    x = 5;
    int *px;
    px = &x;
    *px = 6;
    return *px;
}
