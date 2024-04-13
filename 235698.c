static Jsi_RC CDataConfCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdcGet(cd, _this, funcPtr);
    return CDataOptionsConf(interp, CDataOptions, args, cd, ret, 0, 0);
}