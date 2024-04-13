Math_f sh_mathstdfun(const char *fname, size_t fsize, short *nargs) {
    const struct mathtab *tp;
    char c = fname[0];
    for (tp = shtab_math; *tp->fname; tp++) {
        if (*tp->fname > c) break;
        if (tp->fname[1] == c && tp->fname[fsize + 1] == 0 &&
            strncmp(&tp->fname[1], fname, fsize) == 0) {
            if (nargs) *nargs = *tp->fname;
            return tp->fnptr;
        }
    }
    return NULL;
}