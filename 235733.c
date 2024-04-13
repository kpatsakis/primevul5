Jsi_Obj *Jsi_ObjNewArray(Jsi_Interp *interp, Jsi_Value **items, int count, int copyflag)
{
    Jsi_Obj *obj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
    if (count>=0) {
        int i;
        if (Jsi_ObjArraySizer(interp, obj, count) <= 0) {
            Jsi_ObjFree(interp, obj);
            return NULL;
        }
        for (i = 0; i < count; ++i) {
            if (!items[i]) continue;
            if (!copyflag) {
                obj->arr[i] = items[i];
                Jsi_IncrRefCount(interp, items[i]);
            } else {
                obj->arr[i] = Jsi_ValueNew1(interp);
                Jsi_ValueCopy(interp, obj->arr[i], items[i]);
            }
        }
    }
    obj->arrCnt = count;
    assert(obj->arrCnt<=obj->arrMaxSize);
    return obj;
}