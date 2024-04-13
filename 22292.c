static void annotation_get_lastupdate(annotate_state_t *state,
                                      struct annotate_entry_list *entry)
{
    struct stat sbuf;
    char valuebuf[RFC3501_DATETIME_MAX+1];
    struct buf value = BUF_INITIALIZER;
    char *fname;
    int r;

    r = annotate_state_need_mbentry(state);
    if (r)
        goto out;

    fname = mbentry_metapath(state->mbentry, META_INDEX, 0);
    if (!fname)
        goto out;
    if (stat(fname, &sbuf) == -1)
        goto out;

    time_to_rfc3501(sbuf.st_mtime, valuebuf, sizeof(valuebuf));
    buf_appendcstr(&value, valuebuf);

    output_entryatt(state, entry->name, "", &value);
out:
    buf_free(&value);
}