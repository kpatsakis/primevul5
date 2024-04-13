void Jsi_ObjFree(Jsi_Interp *interp, Jsi_Obj *obj)
{
    interp->dbPtr->objCnt--;
    //assert(obj->refcnt == 0);
    jsi_AllObjOp(interp, obj, 0);
#ifdef JSI_MEM_DEBUG
    if (interp != obj->VD.interp)
        printf("interp mismatch of objFree: %p!=%p : %p\n", interp, obj->VD.interp, obj);
    jsi_DebugObj(obj,"Free", jsi_DebugValueCallIdx(), interp);
    if (obj->VD.hPtr && !interp->cleanup) {
        Jsi_HashEntryDelete(obj->VD.hPtr);
        obj->VD.hPtr = NULL;
    }
#endif
    /* printf("Free obj: %x\n", (int)obj); */
    switch (obj->ot) {
        case JSI_OT_STRING:
            if (!obj->isstrkey)
                Jsi_Free(obj->d.s.str);
            obj->d.s.str = 0;
            obj->isstrkey = 0;
            break;
        case JSI_OT_FUNCTION:
            jsi_FuncObjFree(obj->d.fobj);
            break;
        case JSI_OT_ITER:
            Jsi_IterObjFree(obj->d.iobj);
            break;
        case JSI_OT_USEROBJ:
            jsi_UserObjFree(interp, obj->d.uobj);
        case JSI_OT_ARRAY:
        case JSI_OT_OBJECT:
            break;
        case JSI_OT_REGEXP:
            if ((obj->d.robj->eflags&JSI_REG_STATIC)==0) {
                regfree(&obj->d.robj->reg);
                Jsi_Free(obj->d.robj);
            }
            break;
        default:
            break;
    }
    if (obj->tree)
        Jsi_TreeDelete(obj->tree);
    if (obj->arr) {
        int i = -1;
        while (++i < (int)obj->arrCnt)
            if (obj->arr[i])
                Jsi_DecrRefCount(interp, obj->arr[i]);
        Jsi_Free(obj->arr);
        obj->arr = NULL;
    }
    obj->tree = NULL;
    if (obj->clearProto)
        Jsi_DecrRefCount(interp, obj->__proto__);
#ifdef JSI_MEM_DEBUG
    memset(obj, 0, (sizeof(*obj)-sizeof(obj->VD)));
#endif
    Jsi_Free(obj);
}