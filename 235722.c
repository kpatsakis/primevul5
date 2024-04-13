static Jsi_RC jsi_ValueToIniVal(Jsi_Interp *interp, Jsi_OptionSpec* spec, Jsi_Value *inValue, const char *inStr, void *record, Jsi_Wide flags)
{
    if (inStr)
        return JSI_ERROR;
    uchar *data = (uchar*)record, *odata;
    //Jsi_OptionValue iv;
    int idx = (intptr_t)spec->data;
    if (idx<=0 || !inValue)
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
    if (!tsptr->sfmt) {
        if (tsptr->flags&jsi_CTYP_STRUCT && tsptr->extra) {
            if (!Jsi_ValueIsBoolean(interp, inValue)) Jsi_LogWarn("init for struct must be boolean");
            if (!Jsi_ValueIsTrue(interp, inValue)) return JSI_OK;
            Jsi_StructSpec *tsl = jsi_csGetStructSpec(tsptr->extData);
            int i, n = (sf->arrSize?sf->arrSize:1);
            for (i=0; i<n; i++)
                memcpy(odata+sf->offset+i*tsl->size, tsptr->extra, tsl->size);
            return JSI_OK;
        } else if (!(tsptr->flags&jsi_CTYP_ENUM))
            return Jsi_LogError("init not supported for type: %s", tsptr->cName);
    }
    return jsi_SetOption(interp, sf, "init", odata, inValue, flags, interp->isSafe);
}