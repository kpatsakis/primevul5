static Jsi_RC DeleteTreeValue(Jsi_Interp *interp, Jsi_TreeEntry *ti, void *p) {
    /* Cleanup tree value. */
    SIGASSERT(ti,TREEENTRY);
    Jsi_Value *v = (Jsi_Value*)p;
    SIGASSERT(v,VALUE);
    Jsi_DecrRefCount(interp, v);
    ti->value = NULL;
    return JSI_OK;
}