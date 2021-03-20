{
    struct foo {
        signed const long long long *const volatile const x;
        struct {
            int y;
        } w;
        unsigned char *px;
    } z;
    int zz;
    for (zz = 0; zz < 5; ++zz) {
        int tmp;
        tmp = z.x;
        z.x = z.w.y;
        z.w.y = tmp;
    }
};
//z.x = 2 + 3 * 5 % 6 * (1 / 4 + 3);
/*zz = z.x += 2 + 3 * (4 - 5) % (y ? 6 + 7 : 7 * 8);
c1 ? ++t1-- : c2 ? t2++ : f;
;
&*p++.;
;
foo(a, b, c, d)[2][3];
;;;;
if (x == y) {
    for (i = 0; i < n; ++i)
        printf("%d", i);
};*/