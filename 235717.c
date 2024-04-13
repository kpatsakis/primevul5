Jsi_Obj * Jsi_ObjNew(Jsi_Interp *interp) {
    Jsi_Obj *obj = jsi_ObjNew_(interp);
    jsi_AllObjOp(interp, obj, 1);
    return obj;
}