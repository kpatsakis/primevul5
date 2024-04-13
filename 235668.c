Jsi_Value *jsi_ObjArraySetDup(Jsi_Interp *interp, Jsi_Obj *obj, Jsi_Value *value, int n)
{
    if (Jsi_ObjArraySizer(interp, obj, n) <= 0)
        return NULL;
    if (obj->arr[n])
    {
        Jsi_ValueCopy(interp, obj->arr[n], value);
        return obj->arr[n];
    }
    Assert(obj->arrCnt<=obj->arrMaxSize);
    Jsi_Value *v = Jsi_ValueNew1(interp);
    int m;
    Jsi_ValueCopy(interp,v, value);
    obj->arr[n] = v;
    m = Jsi_ObjGetLength(interp, obj);
    if ((n+1) > m)
       Jsi_ObjSetLength(interp, obj, n+1);
    return v;
}