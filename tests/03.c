struct Foo {
    int x;
} Foo_const(int x) {
    struct Foo f;
    f.x = x;
    return f;
}

int main(int argc, char **argv) {
    return 0;
}
