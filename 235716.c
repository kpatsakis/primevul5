Jsi_Obj *Jsi_ObjNewObj(Jsi_Interp *interp, Jsi_Value **items, int count)
{
    Jsi_Obj *obj = Jsi_ObjNewType(interp, JSI_OT_OBJECT);
    if (count%2) return obj;
    int i;
    for (i = 0; i < count; i += 2) {
        if (!items[i] || !items[i+1]) continue;
        Jsi_Value *v = Jsi_ValueDup(interp, items[i+1]);
        ObjInsertFromValue(interp, obj, items[i], v);
        Jsi_DecrRefCount(interp, v);
    }
    return obj;
}