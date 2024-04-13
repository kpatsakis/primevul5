static void annotation_get_fromfile(annotate_state_t *state,
                                    struct annotate_entry_list *entry)
{
    const char *filename = (const char *) entry->desc->rock;
    char path[MAX_MAILBOX_PATH+1];
    struct buf value = BUF_INITIALIZER;
    FILE *f;

    snprintf(path, sizeof(path), "%s/msg/%s", config_dir, filename);
    if ((f = fopen(path, "r")) && buf_getline(&value, f)) {

        /* TODO: we need a buf_chomp() */
        if (value.s[value.len-1] == '\r')
            buf_truncate(&value, value.len-1);
    }
    if (f) fclose(f);
    output_entryatt(state, entry->name, "", &value);
    buf_free(&value);
}