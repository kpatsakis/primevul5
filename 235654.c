static Jsi_RC CDataEnumDefineCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                                Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_RC rc = JSI_OK;
    char **argv = NULL, *cmt, *cp;
    Jsi_OptionTypedef *st = NULL;
    jsi_csInitType(interp);
    Jsi_Value *val = Jsi_ValueArrayIndex(interp, args, 0),
        *flds = Jsi_ValueArrayIndex(interp, args, 1);
    int vlen, flen, i, argc;
    Jsi_DString fStr = {};
    const char **el, *vstr = Jsi_ValueString(interp, val, &vlen),
        *fstr = Jsi_ValueString(interp, flds, &flen);
    if (vstr) {
        cmt = Jsi_Strstr(fstr, "//");
        Jsi_DString tStr = {fstr};
        fstr=jsi_TrimStr(Jsi_DSValue(&tStr));
        Jsi_SplitStr(fstr, &argc, &argv, (cmt?"\n":","), &fStr);
        Jsi_DSFree(&tStr);
    } else if (!Jsi_ValueIsArray(interp,flds) || (argc=Jsi_ValueGetLength(interp, flds))<1)
        return Jsi_LogError("arg 2 must be string or non-empty array");

    Jsi_EnumSpec *sl, *sf, recs[argc+1];
    memset(recs, 0, sizeof(recs));
    sl = recs+argc;
    SIGINIT(sl, OPTS_ENUM);
    if (vstr) {
        sl->name = Jsi_KeyAdd(interp, vstr);
    } else if (Jsi_OptionsProcess(interp, EnumOptions, sl, val, 0) < 0) {
        rc = JSI_ERROR;
        goto bail;
    }
    if (!Jsi_StrIsAlnum(sl->name)) {
        rc = Jsi_LogError("invalid enum name: %s", sl->name);
        goto bail;
    }
    if (jsi_csEnumGet(interp, sl->name)) {
        rc = Jsi_LogError("enum already exists: %s", sl->name);
        goto bail;
    }
    for (i = 0; i<argc; i++) {
        sf = recs+i;
        SIGINIT(sf, OPTS_FIELD);
        sf->idx = i;
        if (i)
            sf->value = recs[i-1].value+1;
        if (!argv)
            val = Jsi_ValueArrayIndex(interp, flds, i);
        else {
            if (cmt) {
                cp = Jsi_Strstr(argv[i], "//"); 
                if (cp) {
                    *cp = 0;
                    cp += 2;
                    cp = jsi_TrimStr(cp);
                    sf->help = Jsi_KeyAdd(interp, cp);
                }
            }
            cp = Jsi_Strchr(argv[i], ','); 
            if (cp) *cp = 0;
            cp = Jsi_Strchr(argv[i], '='); 
            if (cp) {
                *cp++ = 0;
                cp = jsi_TrimStr(cp);
                if (Jsi_GetWide(interp, cp, &sf->value, 0) != JSI_OK) {
                    rc = Jsi_LogError("Bad value");
                    goto bail;
                }
            }
            cp = jsi_TrimStr(argv[i]);
            sf->name = Jsi_KeyAdd(interp, cp);
            val = NULL;
        }
        if (val && Jsi_OptionsProcess(interp, EnumFieldOptions, sf, val, 0) < 0) {
            rc = JSI_ERROR;
            goto bail;
        }
        if (Jsi_HashGet(interp->EnumItemHash, sf->name, 0)) {
            rc = Jsi_LogError("duplicate enum item: %s", sf->name);
            goto bail;
        }
        if (!Jsi_StrIsAlnum(sf->name)) {
            rc = Jsi_LogError("invalid enum item name: %s", sf->name);
            goto bail;
        }
    }
    st = (typeof(st))Jsi_Calloc(1, sizeof(*st) + sizeof(char*)*(argc+1)+sizeof(recs));
    SIGINIT(st, TYPEDEF);
    sf = (typeof(sf))((uchar*)(st + 1));
    sl = sf+argc;
    el = (typeof(el))(sl + 1);
    memcpy(sf, recs, sizeof(recs));
    for (i = 0; i<argc; i++)
        el[i] = sf[i].name;
    sl->id = JSI_OPTION_END;
    sl->extData = (uchar*)sf;
    sl->data = el;
    rc = jsi_csSetupEnum(interp, sl, sf, st);
bail:
    Jsi_DSFree(&fStr);
    if (rc != JSI_OK && st)
        Jsi_Free(st);
    return rc;
}