static Jsi_RC CDataEnumFindCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                                Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    Jsi_Value *arg2 = Jsi_ValueArrayIndex(interp, args, 1);
    char *arg1 = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_EnumSpec *ei, *el;
    if (!(el = jsi_csEnumGet(interp, arg1)))
        return Jsi_LogError("Unknown enum: %s", arg1);
    Jsi_Wide wval;
    if (Jsi_GetWideFromValue(interp, arg2, &wval) != JSI_OK)
        return JSI_ERROR;

    ei = (typeof(ei))el->extData;
    uint i;
    for (i=0; i<el->size; i++) {
        if (wval == (Jsi_Wide)ei[i].value) {
            Jsi_ValueMakeStringKey(interp, ret, ei[i].name);
            return JSI_OK;
        }
    }
    return JSI_OK;
}