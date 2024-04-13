static_fn Namval_t *scope(Namval_t *np, struct lval *lvalue, int assign) {
    int flag = lvalue->flag;
    char *sub = 0, *cp = (char *)np;
    Namval_t *mp;
    Shell_t *shp = lvalue->shp;
    int c = 0, nosub = lvalue->nosub;
    Dt_t *sdict = (shp->st.real_fun ? shp->st.real_fun->sdict : 0);
    Dt_t *nsdict = (shp->namespace ? nv_dict(shp->namespace) : 0);
    Dt_t *root = shp->var_tree;

    nvflag_t nvflags = assign ? NV_ASSIGN : 0;
    lvalue->nosub = 0;
    if (nosub < 0 && lvalue->ovalue) return (Namval_t *)lvalue->ovalue;
    lvalue->ovalue = NULL;
    if (cp >= lvalue->expr && cp < lvalue->expr + lvalue->elen) {
        int offset;
        // Do binding to node now.
        int d = cp[flag];
        cp[flag] = 0;
        np = nv_open(cp, root, nvflags | NV_VARNAME | NV_NOADD | NV_NOFAIL);
        if ((!np || nv_isnull(np)) && sh_macfun(shp, cp, offset = stktell(shp->stk))) {
            Fun = sh_arith(shp, sub = stkptr(shp->stk, offset));
            STORE_VT(FunNode.nvalue, sfdoublep, &Fun);
            FunNode.nvshell = shp;
            nv_onattr(&FunNode, NV_NOFREE | NV_LDOUBLE | NV_RDONLY);
            cp[flag] = d;
            return &FunNode;
        }
        if (!np && assign) {
            np = nv_open(cp, root, nvflags | NV_VARNAME);
        }
        cp[flag] = d;
        if (!np) return 0;
        root = shp->last_root;
        if (cp[flag + 1] == '[') {
            flag++;
        } else {
            flag = 0;
        }
    }

    if ((lvalue->emode & ARITH_COMP) && dtvnext(root)) {
        mp = nv_search_namval(np, sdict ? sdict : root, NV_NOSCOPE);
        if (!mp && nsdict) mp = nv_search_namval(np, nsdict, 0);
        if (mp) np = mp;
    }

    while (nv_isref(np)) {
        sub = nv_refsub(np);
        np = nv_refnode(np);
        if (sub) nv_putsub(np, sub, 0, assign ? ARRAY_ADD : 0);
    }

    if (!nosub && flag) {
        int hasdot = 0;
        cp = (char *)&lvalue->expr[flag];
        if (sub) goto skip;
        sub = cp;
        while (1) {
            Namarr_t *ap;
            Namval_t *nq;
            cp = nv_endsubscript(np, cp, 0, shp);
            if (c || *cp == '.') {
                c = '.';
                while (*cp == '.') {
                    hasdot = 1;
                    cp++;
                    while (c = mb1char(&cp), isaname(c)) {
                        ;  // empty body
                    }
                }
                if (c == '[') continue;
            }
            flag = *cp;
            *cp = 0;
            if (c || hasdot) {
                sfprintf(shp->strbuf, "%s%s%c", nv_name(np), sub, 0);
                sub = sfstruse(shp->strbuf);
            }
            if (strchr(sub, '$')) sub = sh_mactrim(shp, sub, 0);
            *cp = flag;
            if (c || hasdot) {
                np = nv_open(sub, shp->var_tree, NV_VARNAME | nvflags);
                return np;
            }
            cp = nv_endsubscript(np, sub, (assign ? NV_ADD : 0) | NV_SUBQUOTE, np->nvshell);
            if (*cp != '[') break;
        skip:
            nq = nv_opensub(np);
            if (nq) {
                np = nq;
            } else {
                ap = nv_arrayptr(np);
                if (ap && !ap->table) {
                    ap->table = dtopen(&_Nvdisc, Dtoset);
                    dtuserdata(ap->table, shp, 1);
                }
                if (ap && ap->table && (nq = nv_search(nv_getsub(np), ap->table, NV_ADD))) {
                    nq->nvenv = np;
                }
                if (nq && nv_isnull(nq)) np = nv_arraychild(np, nq, 0);
            }
            sub = cp;
        }
    } else if (nosub > 0) {
        nv_putsub(np, NULL, nosub - 1, 0);
    }
    return np;
}