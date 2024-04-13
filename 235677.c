Jsi_HashEntry *jsi_csfindInHash(Jsi_Interp *interp, Jsi_Hash * tbl, const char *name)
{
    int len;
    Jsi_HashSearch se;
    Jsi_HashEntry *sentry = 0, *entry = Jsi_HashEntryFind(tbl, name);
    if (entry)
        return entry;
    len = Jsi_Strlen(name);
    entry = Jsi_HashSearchFirst(tbl, &se);
    while (entry) {
        char *ename = (char *) Jsi_HashKeyGet(entry);
        if (!Jsi_Strncmp(name, ename, len)) {
            if (sentry)
                return 0;
            sentry = entry;
        }
        entry = Jsi_HashSearchNext(&se);
    }
    return sentry;
}