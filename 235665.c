static Jsi_RC jsi_csMapFree(Jsi_Interp *interp, Jsi_MapEntry *hPtr, void *ptr) {
    if (!ptr) return JSI_OK;
    Jsi_Free(ptr);
    return JSI_OK;
}