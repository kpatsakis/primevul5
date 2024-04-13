Jsi_Value *jsi_ObjValueNew(Jsi_Interp *interp)
{
    return Jsi_ValueMakeObject(interp, NULL, Jsi_ObjNew(interp));
}