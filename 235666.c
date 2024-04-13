Jsi_Value *Jsi_TreeObjGetValue(Jsi_Obj* obj, const char *key, int isstrkey) {
    Jsi_Tree *treePtr = obj->tree;
    
    if (!isstrkey) {
        Jsi_MapEntry *hPtr = Jsi_MapEntryFind(treePtr->opts.interp->strKeyTbl, key);
        if (!hPtr)
            return NULL;
        key = (const char*)Jsi_MapKeyGet(hPtr, 0);
    }
    Jsi_Value *v = (Jsi_Value*)Jsi_TreeGet(treePtr, (void*)key, 0);
    return v;
}