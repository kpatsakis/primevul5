bool Jsi_ObjIsArray(Jsi_Interp *interp, Jsi_Obj *o)  {
    return ((o)->ot == JSI_OT_OBJECT && o->isarrlist);
}