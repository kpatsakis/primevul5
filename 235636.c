static Jsi_RC CDataGetCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                             Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdcGet(cd, _this, funcPtr);
    uchar *dptr = NULL;
    Jsi_Value *karg = Jsi_ValueArrayIndex(interp, args, 0);
    char kbuf[BUFSIZ];
    void *key = kbuf;
    bool isNull = (!karg || Jsi_ValueIsNull(interp, karg));
    if (isNull) {
        if (cd->mapPtr || cd->arrSize>1)
            return Jsi_LogError("null key used with c-array/map");
    } else {
        if (!cd->mapPtr && cd->arrSize<=0)
            return Jsi_LogError("must be array/map");
        if (JSI_OK != jsi_csGetKey(interp, cd, karg, &key, sizeof(kbuf), 1))
        return JSI_ERROR;
    }

    dptr = (uchar*)cd->data;
    if (isNull) {
    } else if (cd->mapPtr) {
        Jsi_MapEntry *mPtr = Jsi_MapEntryFind(*cd->mapPtr, key);
        if (mPtr)
            dptr = (uchar*)Jsi_MapValueGet(mPtr);
        else
            return Jsi_LogError("arg 1: key not found [%s]", Jsi_ValueToString(interp, karg, NULL));
    } /*else if (!cd->arrSize)
        return Jsi_LogError("arg 2: expected a c-array or map");*/
    else {
        uint kind = (intptr_t)key;
        if (kind>=cd->arrSize)
            return Jsi_LogError("array index out of bounds: %d not in 0,%d", kind, cd->arrSize-1);

        dptr = ((uchar*)cd->data) + cd->sl->size*kind;
        if (cd->isPtrs)
            dptr = ((uchar*)cd->data) + sizeof(void*)*kind;
        else if (cd->isPtr2) {
            dptr = (uchar*)(*(void**)dptr);
            dptr += sizeof(void*)*kind;
        }
    }
    int argc = Jsi_ValueGetLength(interp, args);
    if (argc > 2)
        return Jsi_LogError("expected 0 - 2 args");
    Jsi_Value *arg2 = Jsi_ValueArrayIndex(interp, args, 1);
    return Jsi_OptionsConf(interp, (Jsi_OptionSpec*)cd->sf, dptr, arg2, ret, 0);
}