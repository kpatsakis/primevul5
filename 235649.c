static Jsi_RC jsi_csFieldTypeToValue(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value **outValue, Jsi_DString *outStr, void *record, Jsi_Wide flags)
{
    if (outStr)
        return JSI_ERROR;
    const Jsi_OptionTypedef **sp = (typeof(sp))(((uchar*)record) + spec->offset), *sptr = *sp;
    //const char **s = (const char**)((char*)record + spec->offset);
    if (sptr)
        SIGASSERT(sptr, TYPEDEF);
    if (sptr && sptr->cName)
        Jsi_ValueMakeStringKey(interp, outValue, sptr->cName);
    else
        Jsi_ValueMakeNull(interp, outValue);
    return JSI_OK;
}