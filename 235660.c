void Jsi_ObjSetLength(Jsi_Interp *interp, Jsi_Obj *obj, uint len)
{
    if (obj->isarrlist) {
        assert(len<=obj->arrMaxSize);
        obj->arrCnt = len;
        return;
    }
    Jsi_Value *r = Jsi_TreeObjGetValue(obj,"length", 0);
    if (!r) {
        Jsi_Value *n = Jsi_ValueMakeNumber(interp, NULL, len);
        Jsi_ObjInsert(interp, obj, "length", n, JSI_OM_DONTDEL | JSI_OM_DONTENUM | JSI_OM_READONLY);
    } else {
        Jsi_ValueReset(interp, &r);
        Jsi_ValueMakeNumber(interp, &r, len);
    }
}