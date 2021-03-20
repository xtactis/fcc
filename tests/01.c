typedef struct {
    int x;
    union {
        int int_val;
        double real_val;
    };
} Struct;

int main() {
    char c = '\n';
    const char *str = "sdf fdfs sdsf";
    return 42 + str[6] / c;
}
