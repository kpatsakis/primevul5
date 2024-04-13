Jsi_EnumSpec *jsi_csEnumGet(Jsi_Interp *interp, const char *name)
{
    Jsi_EnumSpec *sl, *spec = jsi_csGetEnum(interp, name);
    if (spec) return spec;

    Jsi_CData_Static *CData_Strs = interp->statics;
    while (CData_Strs) {
        sl = CData_Strs->enums;
        while (sl->name) {
            if (!Jsi_Strcmp(name, sl->name))
                return sl;
            sl++;
        }
        CData_Strs = CData_Strs->nextPtr;
    }
    return NULL;
}