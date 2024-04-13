static Jsi_RC CDataEnumValueCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                               Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    const char *arg1, *arg2;
    Jsi_EnumSpec *ei, *el = 0;
    arg1 = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    arg2 = Jsi_ValueArrayIndexToStr(interp, args, 1, NULL);
    if (!(el = jsi_csEnumGet(interp, arg1))) {
        return JSI_OK;
    }
    if (!(ei = jsi_csEnumGetItem(interp, arg2, el)))
        return JSI_OK;
    Jsi_ValueMakeNumber(interp, ret, ei->value);
    return JSI_OK;
}