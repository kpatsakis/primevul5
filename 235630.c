static Jsi_RC CDataSetCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                             Jsi_Value **ret, Jsi_Func *funcPtr) {
    return CDataSetCmd_(interp, args, _this, ret, funcPtr, 1);
}