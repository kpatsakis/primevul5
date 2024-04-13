void *sh_arithcomp(Shell_t *shp, char *str) {
    const char *ptr = str;
    Arith_t *ep;

    ep = arith_compile(shp, str, (char **)&ptr, arith, ARITH_COMP | 1);
    if (*ptr) errormsg(SH_DICT, ERROR_exit(1), e_lexbadchar, *ptr, str);
    return ep;
}