Jsi_RC Jsi_ObjArrayAdd(Jsi_Interp *interp, Jsi_Obj *o, Jsi_Value *v)
{
    if (o->isarrlist == 0)
        return JSI_ERROR;
    if (!o->arr)
        Jsi_ObjListifyArray(interp, o);
    int len = o->arrCnt;
    if (Jsi_ObjArraySizer(interp, o, len+1) <= 0)
        return JSI_ERROR;
    o->arr[len] = v;
    if (v)
        Jsi_IncrRefCount(interp, v);
    assert(o->arrCnt<=o->arrMaxSize);
    return JSI_OK;
}