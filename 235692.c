static Jsi_RC CDataEnumFieldConfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    Jsi_EnumSpec *ei, *sf;
    char *arg1 = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    if (!(sf = jsi_csEnumGet(interp, arg1)))
        return Jsi_LogError("unknown enum item: %s", arg1);
    ei = 0;
    char *arg2 = Jsi_ValueArrayIndexToStr(interp, args, 1, NULL);
    if (!(ei = jsi_csEnumGetItem(interp, arg2, sf)))
        return JSI_OK;

    return CDataOptionsConf(interp, EnumFieldOptions, args, ei, ret, 0, 2);
}