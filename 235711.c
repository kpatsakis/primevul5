static Jsi_RC jsi_csObjFree(Jsi_Interp *interp, void *data)
{
    CDataObj *cd = (CDataObj *)data;
    cd->sl->value--;
    if (cd->slKey)
        cd->slKey->value--;
    if (cd->isAlloc) {
        if (cd->mapPtr && *cd->mapPtr) {
            Jsi_MapDelete(*cd->mapPtr);
            *cd->mapPtr = NULL;
        }
        else if (cd->data) Jsi_Free(cd->data);
    }
    Jsi_Free(cd);
    return JSI_OK;
}