static Jsi_RC CDataEnumGetCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    
    char *name = Jsi_ValueArrayIndexToStr(interp, args, 0, NULL);
    Jsi_EnumSpec *sl = jsi_csEnumGet(interp, name);
    if (!sl)
        return JSI_OK;
    JSI_DSTRING_VAR(dsPtr, 400);
    Jsi_RC rc = CDataEnumGetDfn(interp, sl, dsPtr);
    if (JSI_OK == rc)
        rc = Jsi_JSONParse(interp, Jsi_DSValue(dsPtr), ret, 0);
    Jsi_DSFree(dsPtr);
    return rc;
}