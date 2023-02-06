int main() {
    int x;
    int y;
    int z;
    if (x > 5 && y > 5) {
        z = x * y;
        if (z < 100) {
            z *= 2;
        }
    } else if (x > 5) {
        y = -1;
        z = x * x;
        if (z < 100) {
            z *= 2;
        }
    } else {
        return -1;
    }

    if (y == -1) {
        x = -x;
    } else {
        return z;
    }

    return x*x + z;
}
