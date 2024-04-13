EXPORTED int specialuse_validate(const char *mboxname, const char *userid,
                                 const char *src, struct buf *dest)
{
    const char *specialuse_extra_opt = config_getstring(IMAPOPT_SPECIALUSE_EXTRA);
    char *strval = NULL;
    strarray_t *valid = NULL;
    strarray_t *new_attribs = NULL;
    strarray_t *cur_attribs = NULL;
    struct buf mbattribs = BUF_INITIALIZER;
    int i, j;
    int r = 0;

    if (!src) {
        buf_reset(dest);
        return 0;
    }

    /* If there is a valid mboxname, we get the current specialuse annotations.
     */
    if (mboxname) {
        annotatemore_lookup(mboxname, "/specialuse", userid, &mbattribs);
        if (mbattribs.len) {
            cur_attribs = strarray_split(buf_cstring(&mbattribs), NULL, 0);
        }
    }

    /* check specialuse_extra option if set */
    if (specialuse_extra_opt)
        valid = strarray_split(specialuse_extra_opt, NULL, 0);
    else
        valid = strarray_new();

    /* strarray_add(valid, "\\All"); -- we don't support virtual folders right now */
    strarray_add(valid, "\\Archive");
    strarray_add(valid, "\\Drafts");
    /* strarray_add(valid, "\\Flagged"); -- we don't support virtual folders right now */
    strarray_add(valid, "\\Important"); // draft-ietf-specialuse-important
    strarray_add(valid, "\\Junk");
    strarray_add(valid, "\\Sent");
    strarray_add(valid, "\\Trash");
    strarray_add(valid, "\\Snoozed"); // JMAP

    new_attribs = strarray_split(src, NULL, 0);

    for (i = 0; i < new_attribs->count; i++) {
        int skip_mbcheck = 0;
        const char *item = strarray_nth(new_attribs, i);

        for (j = 0; j < valid->count; j++) { /* can't use find here */
            if (!strcasecmp(strarray_nth(valid, j), item))
                break;
            /* or without the leading '\' */
            if (!strcasecmp(strarray_nth(valid, j) + 1, item))
                break;
        }

        if (j >= valid->count) {
            r = IMAP_ANNOTATION_BADENTRY;
            goto done;
        }

        if (cur_attribs &&
            (strarray_find_case(cur_attribs, strarray_nth(valid, j), 0) >= 0)) {
            /* The mailbox has this specialuse attribute set already */
            skip_mbcheck = 1;
        }

        /* don't allow names that are already in use */
        if (!skip_mbcheck) {
            char *mbname = mboxlist_find_specialuse(strarray_nth(valid, j),
                                                    userid);
            if (mbname) {
                free(mbname);
                r = IMAP_MAILBOX_SPECIALUSE;
                goto done;
            }
        }

        /* normalise the value */
        strarray_set(new_attribs, i, strarray_nth(valid, j));
    }

    strval = strarray_join(new_attribs, " ");
    buf_setcstr(dest, strval);

done:
    free(strval);
    strarray_free(valid);
    strarray_free(new_attribs);
    strarray_free(cur_attribs);
    buf_free(&mbattribs);
    return r;
}