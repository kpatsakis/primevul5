static Jsi_RC jsi_IniValToValue(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value **outValue, Jsi_DString *outStr, void *record, Jsi_Wide flags)
{
    if (outStr)
        return JSI_ERROR;
    uchar *data = (uchar*)record, *odata;
    int idx = (intptr_t)spec->data;
    if (idx<=0 || !outValue)
        return Jsi_LogError("idx/value not set");
    Jsi_OptionSpec *typePtr = spec-idx;
    if (typePtr->id != JSI_OPTION_CUSTOM || typePtr->custom != &jsi_OptSwitchFieldType)
        return Jsi_LogError("Bad field type");
    const Jsi_OptionTypedef **tsp = (typeof(tsp))(data + typePtr->offset), *tsptr = *tsp, *ssp;
    SIGASSERT(tsptr, TYPEDEF);
    Jsi_FieldSpec *sf = jsi_csGetFieldSpec(data);
    Jsi_StructSpec *sl = jsi_csGetStructSpec(sf->extData);
    ssp = sl->type;
    SIGASSERT(ssp, TYPEDEF);
    odata = ssp->extra;
    if (odata && tsptr->sfmt)
        return jsi_GetOption(interp, sf, odata, "init", outValue, flags);

    Jsi_ValueMakeNull(interp, outValue);
    return JSI_OK;
    
    //Jsi_OptionSpec ospec = {.sig=JSI_SIG_OPTS, .id=tsptr->id, .name="init", .offset=0, .size=sizeof(Jsi_OptionValue) };
    //return jsi_GetOption(interp, &ospec, odata, ospec.name, outValue, flags);
}