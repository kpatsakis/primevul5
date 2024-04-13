static Jsi_RC CDataUnsetCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdcGet(cd, _this, funcPtr);
    Jsi_Value *arg2 = Jsi_ValueArrayIndex(interp, args, 0);   
    char kbuf[BUFSIZ];
    void *key = kbuf;
    if (!cd->mapPtr && cd->arrSize<=0)
        return Jsi_LogError("can not call unset on non map/array");
    if (JSI_OK != jsi_csGetKey(interp, cd, arg2, &key, sizeof(kbuf), 2)) {
        return JSI_ERROR;
    }
    uchar *dptr = NULL;
    Jsi_MapEntry *mPtr = Jsi_MapEntryFind(*cd->mapPtr, key);
    if (mPtr)
        dptr = (uchar*)Jsi_MapValueGet(mPtr);
    if (!dptr) {
        if (cd->keyType != JSI_KEYS_ONEWORD)
            return Jsi_LogError("no data in map: %s", (char*)key);
        else
            return Jsi_LogError("no data in map: %p", key);
        return JSI_ERROR;
    }
    Jsi_Free(dptr);
    Jsi_MapEntryDelete(mPtr);
    return JSI_OK;

}