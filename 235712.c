void Jsi_ObjFromDS(Jsi_DString *dsPtr, Jsi_Obj *obj) {
    int len = Jsi_DSLength(dsPtr);
    if (obj->ot == JSI_OT_STRING && obj->d.s.str && !obj->isstrkey)
        Jsi_Free(obj->d.s.str);
    if (!(obj->d.s.str = (char*)dsPtr->strA)) {
        obj->d.s.str = (char*)Jsi_Malloc(len+1);
        memcpy(obj->d.s.str, dsPtr->Str, len+1);
    }
    obj->d.s.len = len;
    obj->isBlob = 1;
    dsPtr->strA = NULL;
    dsPtr->Str[0] = 0;
    dsPtr->len = 0;
    dsPtr->spaceAvl = dsPtr->staticSize;
}