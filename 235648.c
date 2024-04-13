static Jsi_RC jsi_csSetupEnum(Jsi_Interp *interp, Jsi_EnumSpec *sl, Jsi_FieldSpec *sf, Jsi_OptionTypedef* st) {
    bool isNew;
    int cnt = 0;
    if (Jsi_HashEntryFind(interp->CTypeHash, sl->name))
        return Jsi_LogError("enum is c-type: %s", sl->name);
    Jsi_HashEntry *entry = Jsi_HashEntryNew(interp->EnumHash, sl->name, &isNew);
    if (!isNew)
        return Jsi_LogError("duplicate enum: %s", sl->name);
    Jsi_HashValueSet(entry, sl);
    //Jsi_Number val = 0;
    sl->extData = (uchar*)sf;
    Jsi_Wide maxVal = 0, aval;
    while (sf && sf->id != JSI_OPTION_END) {
        sf->idx = cnt;
        Jsi_HashSet(interp->EnumItemHash, sf->name, sf);
        sf->extData = (uchar*)sl;
        aval =  sf->value;
        if (aval<0) aval = -aval;
        if (aval>maxVal) maxVal = aval;
        sf++, cnt++;
    }
    Jsi_HashSet(interp->EnumHash, sl->name, sl);
    sl->idx = cnt;
    if (!sl->size) 
        sl->size = cnt;
    if (!st) {
        st = (typeof(st))Jsi_Calloc(1, sizeof(*st));
        SIGINIT(st, TYPEDEF);
    }
    st->cName = sl->name;
    st->idName = "CUSTOM";
    st->id = JSI_OPTION_CUSTOM;
    st->size = jsi_NumWideBytes(aval);
    st->flags = jsi_CTYP_DYN_MEMORY|jsi_CTYP_ENUM;
    st->extData = (uchar*)sl;
    Jsi_HashSet(interp->CTypeHash, st->cName, st);
    return JSI_OK;
}