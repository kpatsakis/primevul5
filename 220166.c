static_fn Sfdouble_t number(const char *s, char **p, int b, struct lval *lvalue) {
    Sfdouble_t r;
    char *t;
    int oerrno;
    int c;
    char base;
    struct lval v;

    oerrno = errno;
    errno = 0;
    base = b;

    if (!lvalue) {
        lvalue = &v;
    } else if (lvalue->shp->bltindata.bnode == SYSLET && !sh_isoption(lvalue->shp, SH_LETOCTAL)) {
        while (*s == '0' && isdigit(s[1])) s++;
    }
    lvalue->eflag = 0;
    lvalue->isfloat = 0;
    r = strton64(s, &t, &base, -1);
    if (*t == '8' || *t == '9') {
        base = 10;
        errno = 0;
        r = strton64(s, &t, &base, -1);
    }
    if (base <= 1) base = 10;
    if (*t == '_') {
        if ((r == 1 || r == 2) && strcmp(t, "_PI") == 0) {
            t += 3;
            r = Mtable[(int)r - 1].value;
        } else if (r == 2 && strcmp(t, "_SQRTPI") == 0) {
            t += 7;
            r = Mtable[2].value;
        }
    }
    c = r == LLONG_MAX && errno ? 'e' : *t;
    if (c == getdecimal() || c == 'e' || c == 'E' || (base == 16 && (c == 'p' || c == 'P'))) {
        r = strtold(s, &t);
        lvalue->isfloat = TYPE_LD;
    }
    if (t > s) {
        if (*t == 'f' || *t == 'F') {
            t++;
            lvalue->isfloat = TYPE_F;
            r = (float)r;
        } else if (*t == 'l' || *t == 'L') {
            t++;
            lvalue->isfloat = TYPE_LD;
        } else if (*t == 'd' || *t == 'D') {
            t++;
            lvalue->isfloat = TYPE_LD;
            r = (double)r;
        }
    }
    errno = oerrno;
    *p = t;
    return r;
}