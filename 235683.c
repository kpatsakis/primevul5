Jsi_OptionSpec *jsi_csInitEnumItem(Jsi_Interp *interp)
{
    Jsi_HashEntry *entry;
    bool isNew;
    Jsi_EnumSpec *sl = interp->statics->enums;
    while (sl &&  sl->name && sl->id != JSI_OPTION_END) {
        Jsi_OptionSpec *ei = (typeof(ei))sl->data;
        while (ei->name  && ei->id != JSI_OPTION_END) {
            entry = Jsi_HashEntryNew(interp->EnumItemHash, ei->name, &isNew);
            if (!isNew)
                Jsi_LogBug("duplicate enum item: %s", ei->name);
            Jsi_HashValueSet(entry, ei);
            ei++;
        }
        sl++;
    }
    return 0;
}