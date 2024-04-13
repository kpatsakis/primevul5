static Jsi_RC CDataStructNamesCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                                 Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    int argc = Jsi_ValueGetLength(interp, args);

    if (argc == 0)
        return Jsi_HashKeysDump(interp, interp->StructHash, ret, 0);
    char *name = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_StructSpec *sf, *sl;
    if (name == NULL || !(sl = Jsi_CDataStruct(interp, name)))
        return Jsi_LogError("Unknown struct: %s", name);
    Jsi_ValueMakeArrayObject(interp, ret, NULL);
    sf = (typeof(sf))sl->data;
    int m = 0;
    while (sf && sf->id != JSI_OPTION_END)
    {
        Jsi_ValueArraySet(interp, *ret, Jsi_ValueNewBlobString(interp, sf->name), m++);
        sf++;
    }
    return JSI_OK;
}