static int find_p(void *rock, const char *key, size_t keylen,
                const char *data __attribute__((unused)),
                size_t datalen __attribute__((unused)))
{
    struct find_rock *frock = (struct find_rock *) rock;
    const char *mboxname, *entry, *userid;
    unsigned int uid;
    int r;

    r = split_key(frock->d, key, keylen, &mboxname,
                  &uid, &entry, &userid);
    if (r < 0)
        return 0;

    if (!userid)
        return 0;

    if (frock->uid &&
        frock->uid != ANNOTATE_ANY_UID &&
        frock->uid != uid)
        return 0;
    if (!GLOB_MATCH(frock->mglob, mboxname))
        return 0;
    if (!GLOB_MATCH(frock->eglob, entry))
        return 0;
    return 1;
}