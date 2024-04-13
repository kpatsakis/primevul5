static int make_key(const char *mboxname,
                    unsigned int uid,
                    const char *entry,
                    const char *userid,
                    char *key, size_t keysize)
{
    int keylen;

    if (!uid) {
        strlcpy(key, mboxname, keysize);
    }
    else if (uid == ANNOTATE_ANY_UID) {
        strlcpy(key, "*", keysize);
    }
    else {
        snprintf(key, keysize, "%u", uid);
    }
    keylen = strlen(key) + 1;
    strlcpy(key+keylen, entry, keysize-keylen);
    keylen += strlen(entry);
    /* if we don't have a userid, we're doing a foreach() */
    if (userid) {
        keylen++;
        strlcpy(key+keylen, userid, keysize-keylen);
        keylen += strlen(userid) + 1;
    }

    return keylen;
}