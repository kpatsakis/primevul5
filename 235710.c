static Jsi_RC CDataStructUndefineCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    char *name = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_HashEntry *entry = NULL;
    if (name)
        entry = Jsi_HashEntryFind(interp->StructHash, name);
    if (!entry)
        return Jsi_LogError("Unknown struct: %s", name);
    Jsi_StructSpec *sl = (typeof(sl))Jsi_HashValueGet(entry);
    if (sl->value)
        return Jsi_LogError("Struct in use: %d", (int)sl->value);
    Jsi_HashEntryDelete(entry);
    entry = Jsi_HashEntryFind(interp->CTypeHash, name);
    if (entry)
        Jsi_HashEntryDelete(entry);
    return JSI_OK;
}