Jsi_StructSpec *Jsi_CDataStruct(Jsi_Interp *interp, const char *name) {
    return (Jsi_StructSpec *)Jsi_HashGet(interp->StructHash, name, 0);
}