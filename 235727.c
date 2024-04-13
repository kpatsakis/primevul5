static Jsi_RC CDataEnumConfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    Jsi_EnumSpec *sl;
    char *arg1 = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    if (!(sl = jsi_csEnumGet(interp, arg1)))
        return Jsi_LogError("unknown enum: %s", arg1);
    return CDataOptionsConf(interp, EnumOptions, args, sl, ret, 0, 1);
}