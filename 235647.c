Jsi_Obj * jsi_ObjNew(Jsi_Interp *interp, const char *fname, int line, const char *func) {
    Jsi_Obj *obj = jsi_ObjNew_(interp);
    jsi_ValueDebugUpdate(interp, obj, objDebugTbl, fname, line, func);
    jsi_AllObjOp(interp, obj, 1);
    return obj;
}