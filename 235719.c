static Jsi_RC jsi_ArrayFlatSub(Jsi_Interp *interp, Jsi_Obj* nobj, Jsi_Value *arr, int depth) {
    
    int i, n = 0, len = jsi_SizeOfArray(interp, arr->d.obj);
    if (len <= 0) return JSI_OK;
    Jsi_RC rc = JSI_OK;
    int clen = jsi_SizeOfArray(interp, nobj);
    for (i = 0; i < len && rc == JSI_OK; i++) {
        Jsi_Value *t = Jsi_ValueArrayIndex(interp, arr, i);
        if (t && depth>0 && Jsi_ValueIsArray(interp, t))
            rc = jsi_ArrayFlatSub(interp, nobj, t , depth-1);
        else if (!Jsi_ValueIsUndef(interp, t))
            Jsi_ObjArrayAdd(interp, nobj, t);
        if ((uint)(++n + clen)>interp->maxArrayList)
            return Jsi_LogError("array size exceeded");
    }
    return rc;
}