void jsi_csInitEnum(Jsi_Interp *interp)
{
    bool isNew;
    Jsi_EnumSpec *sl = interp->statics->enums;
    while (sl && sl->name && sl->id != JSI_OPTION_END) {
        Jsi_HashEntry *entry = Jsi_HashEntryNew(interp->EnumHash, sl->name, &isNew);
        if (!isNew)
            Jsi_LogBug("duplicate enum: %s", sl->name);
        assert(isNew);
        Jsi_HashValueSet(entry, sl);
        sl++;
    }
}