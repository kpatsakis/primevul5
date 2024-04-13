Jsi_Obj *Jsi_ObjNew(Jsi_Interp *interp) {
    Jsi_Obj* obj = jsi_ObjNew_(interp);
#ifdef JSI_MEM_DEBUG
    jsi_ValueDebugUpdate(interp, obj, objDebugTbl, NULL, 0, NULL);
#endif
    jsi_AllObjOp(interp, obj, 1);
    return obj;
}