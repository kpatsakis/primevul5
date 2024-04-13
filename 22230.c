static int annotate_dbname(const char *mboxname, char **fnamep)
{
    int r = 0;
    mbentry_t *mbentry = NULL;

    if (mboxname) {
        r = mboxlist_lookup(mboxname, &mbentry, NULL);
        if (r) goto out;
    }

    r = annotate_dbname_mbentry(mbentry, fnamep);

out:
    mboxlist_entry_free(&mbentry);
    return r;
}