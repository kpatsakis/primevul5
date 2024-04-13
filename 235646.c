static Jsi_RC jsi_csValueToFieldType(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value *inValue, const char *inStr, void *record, Jsi_Wide flags)
{
    if (inStr)
        return JSI_ERROR;
    const Jsi_OptionTypedef *typ, **sp = (typeof(sp))(((uchar*)record) + spec->offset);

    if (!inStr) {
        if (!inValue || Jsi_ValueIsString(interp, inValue)==0)
            return Jsi_LogError("expected string");
        inStr = Jsi_ValueString(interp, inValue, NULL);
    }
    typ = Jsi_TypeLookup(interp, inStr);
    if (!typ)
        return Jsi_LogError("unknown type: %s", inStr);
    *sp = typ;
    return JSI_OK;
}