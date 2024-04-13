Jsi_CDataDb *Jsi_CDataLookup(Jsi_Interp *interp, const char *name) {
    CDataObj *cd = (typeof(cd))Jsi_UserObjDataFromVar(interp, name);
    if (!cd)
        return NULL;
    return (Jsi_CDataDb*)cd;
}