Jsi_EnumSpec *jsi_csGetEnum(Jsi_Interp *interp, const char *name) {
    return (Jsi_EnumSpec *)Jsi_HashGet(interp->EnumHash, name, 0);
}