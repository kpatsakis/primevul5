Jsi_Obj *jsi_ObjNew_(Jsi_Interp *interp)
{
    Jsi_Obj *obj = (Jsi_Obj*)Jsi_Calloc(1,sizeof(*obj));
    SIGINIT(obj,OBJ);
    jsi_DebugObj(obj,"New", jsi_DebugValueCallIdx(), interp);
    obj->ot = JSI_OT_OBJECT;
    obj->tree = Jsi_TreeNew(interp, JSI_KEYS_STRINGKEY, NULL);
    obj->tree->opts.freeTreeProc = DeleteTreeValue;
    obj->tree->flags.valuesonly = 1;
    obj->__proto__ = interp->Object_prototype;
    interp->dbPtr->objCnt++;
    interp->dbPtr->objAllocCnt++;
   return obj;
}