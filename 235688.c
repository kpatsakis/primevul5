int Jsi_ObjIncrRefCount(Jsi_Interp *interp, Jsi_Obj *obj) {
    jsi_DebugObj(obj,"Incr", jsi_DebugValueCallIdx(), interp);
    SIGASSERT(obj,OBJ);
    Assert(obj->refcnt>=0);
    return ++obj->refcnt;
}