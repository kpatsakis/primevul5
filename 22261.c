static const char *key_as_string(const annotate_db_t *d,
                                 const char *key, int keylen)
{
    const char *mboxname, *entry, *userid;
    unsigned int uid;
    int r;
    static struct buf buf = BUF_INITIALIZER;

    buf_reset(&buf);
    r = split_key(d, key, keylen, &mboxname, &uid, &entry, &userid);
    if (r)
        buf_appendcstr(&buf, "invalid");
    else
        buf_printf(&buf, "{ mboxname=\"%s\" uid=%u entry=\"%s\" userid=\"%s\" }",
                   mboxname, uid, entry, userid);
    return buf_cstring(&buf);
}