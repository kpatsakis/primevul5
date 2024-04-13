static int split_key(const annotate_db_t *d,
                     const char *key, int keysize,
                     const char **mboxnamep,
                     unsigned int *uidp,
                     const char **entryp,
                     const char **useridp)
{
    static struct buf keybuf;
    const char *p;
    const char *end;

    buf_setmap(&keybuf, key, keysize);
    buf_putc(&keybuf, '\0'); /* safety tricks due to broken FM code */
    p = buf_cstring(&keybuf);
    end = p + keysize;

    /*
     * paranoia: split the key into fields on NUL characters.
     * We would use strarray_nsplit() for this, except that
     * by design that function cannot split on NULs and does
     * not handle embedded NULs.
     */

    if (d->mboxname) {
        *mboxnamep = d->mboxname;
        *uidp = 0;
        while (*p && p < end) *uidp = (10 * (*uidp)) + (*p++ - '0');
        if (p < end) p++;
        else return IMAP_ANNOTATION_BADENTRY;
    }
    else {
        /* global db for mailnbox & server scope annotations */
        *uidp = 0;
        *mboxnamep = p;
        while (*p && p < end) p++;
        if (p < end) p++;
        else return IMAP_ANNOTATION_BADENTRY;
    }

    *entryp = p; /* XXX: trailing NULLs on non-userid keys?  Bogus just at FM */
    while (*p && p < end) p++;
    if (p < end && !*p)
        *useridp = p+1;
    else
        *useridp = NULL;
    return 0;
}