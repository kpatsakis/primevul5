static Jsi_RC CDataStructConfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                                Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    char *arg1 = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_StructSpec *sl = jsi_csStructGet(interp, arg1);
    if (!sl)
        return Jsi_LogError("unknown struct: %s", arg1);
    return CDataOptionsConf(interp, StructOptions, args, sl, ret, 0, 1);
}