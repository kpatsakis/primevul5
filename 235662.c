static Jsi_Value *jsi_csFmtKeyCmd(Jsi_MapEntry* hPtr, Jsi_MapOpts *opts, int flags)
{
    void *rec = (opts->mapType==JSI_MAP_HASH ? Jsi_HashKeyGet((Jsi_HashEntry*)hPtr): Jsi_TreeKeyGet((Jsi_TreeEntry*)hPtr));
    if (!rec) return NULL;
    CDataObj *cd = (typeof(cd))opts->user;
    assert(cd);
    Jsi_Interp *interp = cd->interp;
    if (!cd->slKey || !cd->slKey)
        return NULL;
    Jsi_Value *v = Jsi_ValueNew1(interp);
    if (Jsi_OptionsConf(interp, (Jsi_OptionSpec*)cd->keysf, rec, NULL, &v, flags) == JSI_OK)
        return v;
    Jsi_DecrRefCount(interp, v);
    return NULL;
}