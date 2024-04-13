static Jsi_RC CDataTypeConfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    char *arg1 = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_OptionTypedef *nd = NULL;
    jsi_csInitType(interp);
    if (arg1)
        nd = (typeof(nd))Jsi_TypeLookup(interp, arg1);
    if (!nd)
        return Jsi_LogError("Unknown type: %s", arg1);
    return CDataOptionsConf(interp, TypeOptions, args, nd, ret, 0, 1);
}