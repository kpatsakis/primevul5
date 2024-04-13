static_fn Sfdouble_t arith(const char **ptr, struct lval *lvalue, int type, Sfdouble_t n) {
    Shell_t *shp = lvalue->shp;
    Sfdouble_t r = 0;
    char *str = (char *)*ptr;
    char *cp;

    switch (type) {
        case ASSIGN: {
            Namval_t *np = (Namval_t *)(lvalue->value);
            np = scope(np, lvalue, 1);
            nv_putval(np, (char *)&n, NV_LDOUBLE);
            if (lvalue->eflag) lvalue->ptr = nv_hasdisc(np, &ENUM_disc);
            lvalue->eflag = 0;
            r = nv_getnum(np);
            lvalue->value = (char *)np;
            break;
        }
        case LOOKUP: {
            int c = *str;
            char *xp = str;
            lvalue->value = NULL;
            if (c == '.') str++;
            c = mb1char(&str);
            if (isaletter(c)) {
                Namval_t *np = NULL;
                int dot = 0;
                while (1) {
                    xp = str;
                    while (c = mb1char(&str), isaname(c)) xp = str;
                    str = xp;
                    while (c == '[' && dot == NV_NOADD) {
                        str = nv_endsubscript(NULL, str, 0, shp);
                        c = *str;
                    }
                    if (c != '.') break;
                    dot = NV_NOADD;
                    c = *++str;
                    if (c != '[') continue;
                    str = nv_endsubscript(NULL, cp = str, NV_SUBQUOTE, shp) - 1;
                    if (sh_checkid(cp + 1, NULL)) str -= 2;
                }
                if (c == '(') {
                    int off = stktell(shp->stk);
                    int fsize = str - (char *)(*ptr);
                    const struct mathtab *tp;
                    Namval_t *nq;
                    lvalue->fun = NULL;
                    sfprintf(shp->stk, ".sh.math.%.*s%c", fsize, *ptr, 0);
                    stkseek(shp->stk, off);
                    nq = nv_search(stkptr(shp->stk, off), shp->fun_tree, 0);
                    if (nq) {
                        struct Ufunction *rp = FETCH_VT(nq->nvalue, rp);
                        lvalue->nargs = -rp->argc;
                        lvalue->fun = (Math_f)nq;
                        break;
                    }
                    if (fsize <= (sizeof(tp->fname) - 2)) {
                        lvalue->fun = (Math_f)sh_mathstdfun(*ptr, fsize, &lvalue->nargs);
                    }
                    if (lvalue->fun) break;
                    if (lvalue->emode & ARITH_COMP) {
                        lvalue->value = (char *)e_function;
                    } else {
                        lvalue->value = (char *)ERROR_dictionary(e_function);
                    }
                    return r;
                }
                if ((lvalue->emode & ARITH_COMP) && dot) {
                    lvalue->value = (char *)*ptr;
                    lvalue->flag = str - lvalue->value;
                    break;
                }
                *str = 0;
                if (sh_isoption(shp, SH_NOEXEC)) {
                    np = VAR_underscore;
                } else {
                    int offset = stktell(shp->stk);
                    char *saveptr = stkfreeze(shp->stk, 0);
                    Dt_t *root = (lvalue->emode & ARITH_COMP) ? shp->var_base : shp->var_tree;
                    *str = c;
                    cp = str;
                    while (c == '[' || c == '.') {
                        if (c == '[') {
                            str = nv_endsubscript(np, str, 0, shp);
                            c = *str;
                            if (c != '[' && c != '.') {
                                str = cp;
                                c = '[';
                                break;
                            }
                        } else {
                            dot = NV_NOADD | NV_NOFAIL;
                            str++;
                            xp = str;
                            while (c = mb1char(&str), isaname(c)) xp = str;
                            str = xp;
                        }
                    }
                    *str = 0;
                    cp = (char *)*ptr;
                    Varsubscript = false;
                    if ((cp[0] == 'i' || cp[0] == 'I') && (cp[1] == 'n' || cp[1] == 'N') &&
                        (cp[2] == 'f' || cp[2] == 'F') && cp[3] == 0) {
                        Inf = strtold("Inf", NULL);
                        STORE_VT(Infnod.nvalue, sfdoublep, &Inf);
                        np = &Infnod;
                        np->nvshell = shp;
                        nv_onattr(np, NV_NOFREE | NV_LDOUBLE | NV_RDONLY);
                    } else if ((cp[0] == 'n' || cp[0] == 'N') && (cp[1] == 'a' || cp[1] == 'A') &&
                               (cp[2] == 'n' || cp[2] == 'N') && cp[3] == 0) {
                        NaN = strtold("NaN", NULL);
                        STORE_VT(NaNnod.nvalue, sfdoublep, &NaN);
                        np = &NaNnod;
                        np->nvshell = shp;
                        nv_onattr(np, NV_NOFREE | NV_LDOUBLE | NV_RDONLY);
                    } else {
                        const struct Mathconst *mp = NULL;
                        np = NULL;
                        if (strchr("ELPS12", **ptr)) {
                            for (mp = Mtable; *mp->name; mp++) {
                                if (strcmp(mp->name, *ptr) == 0) break;
                            }
                        }
                        if (mp && *mp->name) {
                            r = mp->value;
                            lvalue->isfloat = TYPE_LD;
                            goto skip2;
                        }
                        if (shp->namref_root && !(lvalue->emode & ARITH_COMP)) {
                            np = nv_open(*ptr, shp->namref_root,
                                         NV_NOREF | NV_VARNAME | NV_NOSCOPE | NV_NOADD | dot);
                        }
                        if (!np) {
                            np = nv_open(*ptr, root, NV_NOREF | NV_VARNAME | dot);
                        }
                        if (!np || Varsubscript) {
                            np = NULL;
                            lvalue->value = (char *)*ptr;
                            lvalue->flag = str - lvalue->value;
                        }
                    }
                skip2:
                    if (saveptr != stkptr(shp->stk, 0)) {
                        stkset(shp->stk, saveptr, offset);
                    } else {
                        stkseek(shp->stk, offset);
                    }
                }
                *str = c;
                if (lvalue->isfloat == TYPE_LD) break;
                if (!np) break;  // this used to also test `&& lvalue->value` but that's redundant
                lvalue->value = (char *)np;
                // Bind subscript later.
                if (nv_isattr(np, NV_DOUBLE) == NV_DOUBLE) lvalue->isfloat = 1;
                lvalue->flag = 0;
                if (c == '[') {
                    lvalue->flag = (str - lvalue->expr);
                    do {
                        while (c == '.') {
                            str++;
                            while (xp = str, c = mb1char(&str), isaname(c)) {
                                ;  // empty body
                            }
                            c = *(str = xp);
                        }
                        if (c == '[') str = nv_endsubscript(np, str, 0, np->nvshell);
                        c = *str;
                    } while (c == '[' || c == '.');
                    break;
                }
            } else {
                r = number(xp, &str, 0, lvalue);
            }
            break;
        }
        case VALUE: {
            Namval_t *np = (Namval_t *)(lvalue->value);
            Namarr_t *ap;
            if (sh_isoption(shp, SH_NOEXEC)) return 0;
            np = scope(np, lvalue, 0);
            if (!np) {
                if (sh_isoption(shp, SH_NOUNSET)) {
                    *ptr = lvalue->value;
                    goto skip;
                }
                return 0;
            }
            lvalue->ovalue = (char *)np;
            if (lvalue->eflag) {
                lvalue->ptr = nv_hasdisc(np, &ENUM_disc);
            } else if ((Namfun_t *)lvalue->ptr && !nv_hasdisc(np, &ENUM_disc) &&
                       !nv_isattr(np, NV_INTEGER)) {
                // TODO: The calloc() below should be considered a bandaid and may not be correct.
                // See https://github.com/att/ast/issues/980. This dynamic allocation may leak some
                // memory but that is preferable to referencing a stack var after this function
                // returns. I think I have addressed this by removing the NV_NOFREE flag but I'm
                // leaving this comment due to my low confidence.
                Namval_t *mp = ((Namfun_t *)lvalue->ptr)->type;
                Namval_t *node = calloc(1, sizeof(Namval_t));
                nv_clone(mp, node, 0);
                nv_offattr(node, NV_NOFREE);
                nv_offattr(node, NV_RDONLY);
                nv_putval(node, np->nvname, 0);

                if (nv_isattr(node, NV_NOFREE)) return nv_getnum(node);
            }
            lvalue->eflag = 0;
            if (((lvalue->emode & 2) || lvalue->level > 1 ||
                 (lvalue->nextop != A_STORE && sh_isoption(shp, SH_NOUNSET))) &&
                nv_isnull(np) && !nv_isattr(np, NV_INTEGER)) {
                *ptr = nv_name(np);
            skip:
                lvalue->value = (char *)ERROR_dictionary(e_notset);
                lvalue->emode |= 010;
                return 0;
            }
            if (lvalue->userfn) {
                ap = nv_arrayptr(np);
                if (ap && (ap->flags & ARRAY_UNDEF)) {
                    r = (Sfdouble_t)(uintptr_t)np;
                    lvalue->isfloat = 5;
                    return r;
                }
            }
            r = nv_getnum(np);
            if (nv_isattr(np, NV_INTEGER | NV_BINARY) == (NV_INTEGER | NV_BINARY)) {
                lvalue->isfloat = (r != (Sflong_t)r) ? TYPE_LD : 0;
            } else if (nv_isattr(np, (NV_DOUBLE | NV_SHORT)) == (NV_DOUBLE | NV_SHORT)) {
                lvalue->isfloat = TYPE_F;
                r = (float)r;
            } else if (nv_isattr(np, (NV_DOUBLE | NV_LONG)) == (NV_DOUBLE | NV_LONG)) {
                lvalue->isfloat = TYPE_LD;
            } else if (nv_isattr(np, NV_DOUBLE) == NV_DOUBLE) {
                lvalue->isfloat = TYPE_D;
                r = (double)r;
            }
            if ((lvalue->emode & ARITH_ASSIGNOP) && nv_isarray(np)) {
                lvalue->nosub = nv_aindex(np) + 1;
            }
            return r;
        }
        case MESSAGE: {
            sfsync(NULL);
            if (lvalue->emode & ARITH_COMP) return -1;

            errormsg(SH_DICT, ERROR_exit((lvalue->emode & 3) != 0), lvalue->value, *ptr);
        }
    }
    *ptr = str;
    return r;
}