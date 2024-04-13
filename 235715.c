int Jsi_ObjGetLength(Jsi_Interp *interp, Jsi_Obj *obj)
{
    if (obj->tree && obj->tree->numEntries) {
        Jsi_Value *r = Jsi_TreeObjGetValue(obj, "length", 0);
        Jsi_Number nr;
        if (r && Jsi_GetNumberFromValue(interp,r, &nr) == JSI_OK) {
            if (Jsi_NumberIsInteger(nr))
                return nr;
        }
    }
    if (obj->arr)
        return obj->arrCnt;

    return 0;
}