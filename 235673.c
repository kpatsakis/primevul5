Jsi_EnumSpec *jsi_csEnumGetItem(Jsi_Interp *interp, const char *name, Jsi_EnumSpec * el)
{
    SIGASSERT(el, OPTS_ENUM);
    int cnt = 0;
    uint len = Jsi_Strlen(name);
    if (!el->extData)
        return 0;
    Jsi_FieldSpec *ff = NULL, *sf = jsi_csGetFieldSpec(el->extData);
    while (sf->id != JSI_OPTION_END) {
        if (!Jsi_Strncmp(name, sf->name, len)) {
            if (!sf->name[len])
                return sf;
            ff = sf;
            cnt++;
        }
        sf++;
    }
    if (cnt == 1)
        return ff;
    return 0;
}