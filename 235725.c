static Jsi_TreeEntry* ObjInsertFromValue(Jsi_Interp *interp, Jsi_Obj *obj, Jsi_Value *keyVal, Jsi_Value *nv)
{
    const char *key = NULL;
    int flags = 0;
    Jsi_DString dStr = {};
    if (keyVal->vt == JSI_VT_STRING) {
        flags = (keyVal->f.bits.isstrkey ? JSI_OM_ISSTRKEY : 0);
        key = keyVal->d.s.str;
    } else if (keyVal->vt == JSI_VT_OBJECT && keyVal->d.obj->ot == JSI_OT_STRING) {
        Jsi_Obj *o = keyVal->d.obj;
        flags = (o->isstrkey ? JSI_OM_ISSTRKEY : 0);
        key = o->d.s.str;
    }
    if (key == NULL)
        key = Jsi_ValueGetDString(interp, keyVal, &dStr, 0);
    return Jsi_ObjInsert(interp, obj, key, nv, flags);
}