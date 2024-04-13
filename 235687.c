int Jsi_ObjDecrRefCount(Jsi_Interp *interp, Jsi_Obj *obj)  {
   // if (interp->cleanup && !jsi_AllObjOp(interp, obj, 2))
   //    return 0;
    SIGASSERT(obj,OBJ);
    if (obj->refcnt<=0) {
#ifdef JSI_MEM_DEBUG
        fprintf(stderr, "Obj decr with ref %d: VD.Idx=%d\n", obj->refcnt, obj->VD.Idx);
#endif
        return -2;
    }
    jsi_DebugObj(obj,"Decr", jsi_DebugValueCallIdx(), interp);
    int nref;
    if ((nref = --obj->refcnt) <= 0) {
        obj->refcnt = -1;
        Jsi_ObjFree(interp, obj);
    }
    return nref;
}