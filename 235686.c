static Jsi_RC CDataEnumNamesCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                               Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    int argc = Jsi_ValueGetLength(interp, args);
    
    if (argc == 0)
        return Jsi_HashKeysDump(interp, interp->EnumHash, ret, 0);
    char *arg1 = Jsi_ValueString(interp, Jsi_ValueArrayIndex(interp, args, 0), NULL);
    Jsi_EnumSpec *s, *sf;
    if (arg1 == NULL || !(s = (Jsi_EnumSpec*)Jsi_HashGet(interp->EnumHash, arg1, 0)))
        return Jsi_LogError("Unknown enum: %s", arg1);
    Jsi_ValueMakeArrayObject(interp, ret, NULL);
    sf = (typeof(sf))s->extData;
    int m = 0;
    while (sf && sf->id != JSI_OPTION_END)
    {
        Jsi_ValueArraySet(interp, *ret, Jsi_ValueNewBlobString(interp, sf->name), m++);
        sf++;
    }
    return JSI_OK;
}