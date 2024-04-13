Jsi_TreeEntry *Jsi_TreeObjSetValue(Jsi_Obj *obj, const char *key, Jsi_Value *val, int isstrkey) {
    Jsi_Tree *treePtr = obj->tree;
    bool isNew;
    Jsi_TreeEntry *hPtr;
    Jsi_Interp *interp = treePtr->opts.interp;
    if (!isstrkey) {
        const char *okey = key;
        Jsi_MapEntry *hePtr = Jsi_MapEntryNew(interp->strKeyTbl, key, &isNew);
        key = (const char*)Jsi_MapKeyGet(hePtr, 0);
        if (!key) {
            Jsi_MapEntry *hePtr = Jsi_MapEntryNew(interp->strKeyTbl, okey, &isNew);
            key = (const char*)Jsi_MapKeyGet(hePtr, 0);
        }
    }
    //return Jsi_TreeSet(treePtr, key, val);
    hPtr = Jsi_TreeEntryNew(treePtr, key, &isNew);
    if (!hPtr)
        return NULL;
    if (val)
        SIGASSERT(val,VALUE);
    if (!isNew)
        Jsi_ValueReplace(interp, (Jsi_Value**)&(hPtr->value), val);
    else {
        hPtr->value = val;
        if (val)
            Jsi_IncrRefCount(interp, val);
    }
//    Jsi_Value *oldVal;  /* FYI: This let kitty.breed() work in tests/proto2.js */
//    Assert(val->refCnt>0);
//    if (!isNew) {
//        oldVal = Jsi_TreeValueGet(hPtr);
//        if (oldVal) {
//            Jsi_ValueReset(interp, &oldVal);
//            Jsi_ValueCopy(interp, oldVal, val);
//        }
//    }
//    else
//        hPtr->value = val;

    return hPtr;
}