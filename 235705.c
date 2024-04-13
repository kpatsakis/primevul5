static Jsi_RC CDataNamesCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr) {
    UdcGet(cd, _this, funcPtr);
    if (cd->mapType != JSI_MAP_NONE)
        return Jsi_MapKeysDump(interp, *cd->mapPtr, ret, 0);
    return Jsi_LogError("not a map");;
}