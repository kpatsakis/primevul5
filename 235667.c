static Jsi_RC jsi_csGetKey(Jsi_Interp *interp, CDataObj *cd, Jsi_Value *arg, void **kPtr, size_t ksize, int anum)
{
    void *kBuf = *kPtr;
    *kPtr = NULL;
    if (!arg)
        return Jsi_LogError("missing key arg");;
    Jsi_Number nval = 0;
    switch (cd->keyType) {
        case JSI_KEYS_STRING:
        case JSI_KEYS_STRINGKEY:
            *kPtr = (void*)Jsi_ValueString(interp, arg, NULL);
            if (!*kPtr)
                return Jsi_LogError("arg %d: expected string key", anum);
            break;
        case JSI_KEYS_ONEWORD:
            if (Jsi_ValueGetNumber(interp, arg, &nval) != JSI_OK)
                return Jsi_LogError("arg %d: expected number key", anum);
            *kPtr = (void*)(uintptr_t)nval;
            break;
        default: {
            if (!cd->slKey) {
badkey:
                return Jsi_LogError("arg %d: expected struct key", anum);
            }
            if (arg->vt == JSI_VT_OBJECT && arg->d.obj->ot == JSI_OT_OBJECT) {
                if (cd->slKey->size>ksize || !kBuf)
                    goto badkey;
                memset(kBuf, 0, cd->slKey->size);
                if (Jsi_OptionsConf(interp, (Jsi_OptionSpec*)cd->keysf, kBuf, arg, NULL, 0) != JSI_OK)
                    return JSI_ERROR;
                *kPtr = kBuf;
            } else
                return Jsi_LogError("arg %d: expected object key", anum);
        }
    }
    return JSI_OK;
}