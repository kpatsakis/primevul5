Jsi_RC Jsi_ObjArraySet(Jsi_Interp *interp, Jsi_Obj *obj, Jsi_Value *value, int arrayindex)
{
    int m, n = arrayindex;
    if (Jsi_ObjArraySizer(interp, obj, n) <= 0)
        return JSI_ERROR;
    if (obj->arr[n] == value)
        return JSI_OK;
    if (obj->arr[n])
        Jsi_DecrRefCount(interp, obj->arr[n]);
    Assert(obj->arrCnt<=obj->arrMaxSize);
    obj->arr[n] = value;
    if (value)
        Jsi_IncrRefCount(interp, value);
    m = Jsi_ObjGetLength(interp, obj);
    if ((n+1) > m)
       Jsi_ObjSetLength(interp, obj, n+1);
    return JSI_OK;
}