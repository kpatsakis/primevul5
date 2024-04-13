static Jsi_RC CDataEnumUndefineCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    char *name = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_HashEntry *entry = NULL;
    Jsi_OptionTypedef *st = NULL;
    if (name) {
        entry = Jsi_HashEntryFind(interp->EnumHash, name);
        st = Jsi_TypeLookup(interp, name);
    }
    if (!entry || !st)
        return Jsi_LogError("Unknown enum: %s", name);
    Jsi_EnumSpec *sf, *sl = (typeof(sl))Jsi_HashValueGet(entry);
    if (sl->value)
        return Jsi_LogError("Enum in use");
    Jsi_HashEntryDelete(entry);
    sf = (typeof(sf))sl->extData;
    while (sf && sf->id != JSI_OPTION_END) {
        entry = Jsi_HashEntryFind(interp->EnumItemHash, name);
        if (entry)
            Jsi_HashEntryDelete(entry);
        sf++;
    }
    entry = Jsi_HashEntryFind(interp->CTypeHash, name);
    if (entry)
        Jsi_HashEntryDelete(entry);
    else
        Jsi_Free(st);
    return JSI_OK;
}