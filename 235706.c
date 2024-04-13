static Jsi_RC jsi_csTypeFree(Jsi_Interp *interp, Jsi_HashEntry *hPtr, void *ptr) {
    if (!ptr) return JSI_OK;
    Jsi_OptionTypedef *type = jsi_csGetTypeSpec(ptr);
    if (type->extData && (type->flags&(jsi_CTYP_ENUM|jsi_CTYP_STRUCT)))
        ((Jsi_OptionSpec*)(type->extData))->value--;
    if (type->flags&jsi_CTYP_DYN_MEMORY)
        Jsi_Free(ptr);
    return JSI_OK;
}