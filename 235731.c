static Jsi_RC CDataTypeNamesCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    jsi_csInitType(interp);
    int argc = Jsi_ValueGetLength(interp, args);
    return Jsi_HashKeysDump(interp, (argc?interp->CTypeHash:interp->TYPEHash), ret, 0);
}