Jsi_FieldSpec *jsi_csFieldGet(Jsi_Interp *interp, const char *name, Jsi_StructSpec * sl)
{
    SIGASSERT(sl, OPTS_STRUCT);
    Jsi_FieldSpec *sf, *ff = 0, *f = jsi_csGetFieldSpec(sl->extData);
    int cnt = 0;
    uint len = Jsi_Strlen(name);
    sf = f;
    SIGASSERT(sf, OPTS_FIELD);
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