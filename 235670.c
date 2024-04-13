static Jsi_RC CDataOptionsConf(Jsi_Interp *interp, Jsi_OptionSpec *specs,  Jsi_Value *args,
    void *rec, Jsi_Value **ret, int flags, int skipArgs)
{
    int argc = Jsi_ValueGetLength(interp, args);
    Jsi_Value *val;
    flags |= JSI_OPTS_IS_UPDATE;

    if (argc == skipArgs)
        return Jsi_OptionsDump(interp, specs, rec, ret, flags);
    val = Jsi_ValueArrayIndex(interp, args, skipArgs);
    Jsi_vtype vtyp = Jsi_ValueTypeGet(val);
    if (vtyp == JSI_VT_STRING) {
        char *str = Jsi_ValueString(interp, val, NULL);
        return Jsi_OptionsGet(interp, specs, rec, str, ret, flags);
    }
    if (vtyp != JSI_VT_OBJECT && vtyp != JSI_VT_NULL)
        return Jsi_LogError("expected string, object, or null");
    if (Jsi_OptionsProcess(interp, specs, rec, val, JSI_OPTS_IS_UPDATE) < 0)
        return JSI_ERROR;
    return JSI_OK;
}