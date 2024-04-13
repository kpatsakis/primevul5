void jsi_csInitType(Jsi_Interp *interp)
{
    if (interp->CTypeHash->numEntries) return;
    bool isNew;
    Jsi_HashEntry *entry;
    const Jsi_OptionTypedef *tl;
    if (!interp->typeInit) {
        int i;
        for (i = JSI_OPTION_BOOL; i!=JSI_OPTION_END; i++) {
            tl = Jsi_OptionTypeInfo((Jsi_OptionId)i);
            entry = Jsi_HashEntryNew(interp->TYPEHash, tl->idName, &isNew);
            if (!isNew)
                Jsi_LogBug("duplicate type: %s", tl->idName);
            Jsi_HashValueSet(entry, (void*)tl);
            if (tl->cName && tl->cName[0])
                Jsi_HashSet(interp->CTypeHash, tl->cName, (void*)tl);
        }
    }
    interp->typeInit = 1;
}