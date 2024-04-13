static Jsi_RC ObjListifyArrayCallback(Jsi_Tree *tree, Jsi_TreeEntry *hPtr, void *data)
{
    if (hPtr->f.bits.isarrlist) {
        Jsi_TreeEntryDelete(hPtr);
        tree->opts.interp->delRBCnt++;
        return JSI_ERROR;
    }
    return JSI_OK;
}