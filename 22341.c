static int find_cb(void *rock, const char *key, size_t keylen,
                   const char *data, size_t datalen)
{
    struct find_rock *frock = (struct find_rock *) rock;
    const char *mboxname, *entry, *userid;
    unsigned int uid;
    char newkey[MAX_MAILBOX_PATH+1];
    size_t newkeylen;
    struct buf value = BUF_INITIALIZER;
    struct annotate_metadata mdata;
    int r;

    assert(keylen < MAX_MAILBOX_PATH);

    r = split_key(frock->d, key, keylen, &mboxname,
                  &uid, &entry, &userid);
    if (r) {
        syslog(LOG_ERR, "find_cb: can't split bogus key %*.s", (int)keylen, key);
        return r;
    }

    newkeylen = make_key(mboxname, uid, entry, userid, newkey, sizeof(newkey));
    if (keylen != newkeylen || strncmp(newkey, key, keylen)) {
        syslog(LOG_ERR, "find_cb: bogus key %s %d %s %s (%d %d)", mboxname, uid, entry, userid, (int)keylen, (int)newkeylen);
    }

    r = split_attribs(data, datalen, &value, &mdata);
    if (r) {
        buf_free(&value);
        return r;
    }
#if DEBUG
    syslog(LOG_ERR, "find_cb: found key %s in %s with modseq " MODSEQ_FMT,
            key_as_string(frock->d, key, keylen), frock->d->filename, mdata.modseq);
#endif

    if (frock->since_modseq && frock->since_modseq >= mdata.modseq) {
#if DEBUG
        syslog(LOG_ERR,"find_cb: ignoring key %s: " " modseq " MODSEQ_FMT " is <= " MODSEQ_FMT,
                key_as_string(frock->d, key, keylen), mdata.modseq, frock->since_modseq);
#endif
        buf_free(&value);
        return 0;
    }

    if (((mdata.flags & ANNOTATE_FLAG_DELETED) || !buf_len(&value)) &&
        !(frock->flags & ANNOTATE_TOMBSTONES)) {
#if DEBUG
    syslog(LOG_ERR, "find_cb: ignoring key %s, tombstones are ignored",
            key_as_string(frock->d, key, keylen));
#endif
        buf_free(&value);
        return 0;
    }

    if (!r) r = frock->proc(mboxname, uid, entry, userid, &value, &mdata,
                            frock->rock);
    buf_free(&value);
    return r;
}