static Jsi_RC CDataStructFieldConfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                                Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    char *arg1 = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_StructSpec *sf, *sl = jsi_csStructGet(interp, arg1);
    if (!sl)
        return Jsi_LogError("unknown struct: %s", arg1);
    char *arg2 = Jsi_ValueArrayIndexToStr(interp, args, 1, NULL);
    if (!arg2 || !(sf = jsi_csFieldGet(interp, arg2, sl)))
        return Jsi_LogError("unknown field: %s", arg2);
    return CDataOptionsConf(interp, StructFieldOptions, args, sf, ret, 0, 2);
}