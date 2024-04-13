Jsi_StructSpec *jsi_csStructGet(Jsi_Interp *interp, const char *name)
{
    if (!name) return NULL;
    Jsi_StructSpec *sl,*spec = Jsi_CDataStruct(interp, name);
    if (spec) return spec;

    Jsi_CData_Static *CData_Strs = interp->statics;
    while (CData_Strs) {
        sl = CData_Strs->structs;
        while (sl->name) {
            if (!Jsi_Strcmp(name, sl->name))
                return sl;
            sl++;
        }
        CData_Strs = CData_Strs->nextPtr;
    }
    return NULL;
}