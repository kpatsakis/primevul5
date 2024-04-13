static Jsi_RC CDataEnumGetDfn(Jsi_Interp *interp, Jsi_EnumSpec * sl, Jsi_DString *dsPtr)
{
    
    Jsi_EnumSpec *sf;
    Jsi_DString eStr = {};

    Jsi_DSAppend(dsPtr, "{ name: \"", sl->name, "\"", NULL);
    if (sl->flags)
        Jsi_DSPrintf(dsPtr, ", flags:%" PRIx64, sl->flags);
    if (sl->help && sl->help[0]) {
        Jsi_DSAppend(dsPtr, ", help:", Jsi_JSONQuote(interp, sl->help, -1, &eStr), NULL);
        Jsi_DSFree(&eStr);
    }
    sf = jsi_csGetFieldSpec(sl->extData);
    Jsi_DSAppend(dsPtr, ", fields:[", NULL);
    while (sf->id != JSI_OPTION_END) {
        Jsi_DSPrintf(dsPtr, " { name:\"%s\", value:%#" PRIx64, sf->name, (int64_t)sf->value);
        if (sf->help && sf->help[0]) {
            Jsi_DSAppend(dsPtr, ", help:", Jsi_JSONQuote(interp, sf->help, -1, &eStr), NULL);
            Jsi_DSFree(&eStr);
        }
        Jsi_DSAppend(dsPtr, "}", NULL);
        sf++;
    }
    Jsi_DSAppend(dsPtr, "]}", NULL);
    return JSI_OK;
}