EXPORTED int annotatemore_findall(const char *mboxname, /* internal */
                         unsigned int uid,
                         const char *entry,
                         modseq_t since_modseq,
                         annotatemore_find_proc_t proc,
                         void *rock,
                         int flags)
{
    char key[MAX_MAILBOX_PATH+1], *p;
    size_t keylen;
    int r;
    struct find_rock frock;

    init_internal();

    assert(mboxname);
    assert(entry);
    frock.mglob = glob_init(mboxname, '.');
    frock.eglob = glob_init(entry, '/');
    frock.uid = uid;
    frock.proc = proc;
    frock.rock = rock;
    frock.since_modseq = since_modseq;
    frock.flags = flags;
    r = _annotate_getdb(mboxname, uid, 0, &frock.d);
    if (r) {
        if (r == CYRUSDB_NOTFOUND)
            r = 0;
        goto out;
    }

    /* Find fixed-string pattern prefix */
    keylen = make_key(mboxname, uid,
                      entry, NULL, key, sizeof(key));

    for (p = key; keylen; p++, keylen--) {
        if (*p == '*' || *p == '%') break;
    }
    keylen = p - key;

    r = cyrusdb_foreach(frock.d->db, key, keylen, &find_p, &find_cb,
                        &frock, tid(frock.d));

out:
    glob_free(&frock.mglob);
    glob_free(&frock.eglob);
    annotate_putdb(&frock.d);

    return r;
}