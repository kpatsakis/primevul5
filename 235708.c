static Jsi_FieldSpec*  jsi_csStructFields(Jsi_Interp *interp, const char *name) {
    Jsi_StructSpec* sp = jsi_csStructGet(interp, name);
    if (!sp)
        return NULL;
    return jsi_csGetFieldSpec(sp->extData);
}