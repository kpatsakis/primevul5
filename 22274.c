static int annotation_set_tofile(annotate_state_t *state
                                    __attribute__((unused)),
                                 struct annotate_entry_list *entry,
                                 int maywrite)
{
    const char *filename = (const char *)entry->desc->rock;
    char path[MAX_MAILBOX_PATH+1];
    int r;
    FILE *f;

    if (!maywrite) return IMAP_PERMISSION_DENIED;

    snprintf(path, sizeof(path), "%s/msg/%s", config_dir, filename);

    /* XXX how do we do this atomically with other annotations? */
    if (entry->shared.s == NULL)
        return unlink(path);
    else {
        r = cyrus_mkdir(path, 0755);
        if (r)
            return r;
        f = fopen(path, "w");
        if (!f) {
            syslog(LOG_ERR, "cannot open %s for writing: %m", path);
            return IMAP_IOERROR;
        }
        fwrite(entry->shared.s, 1, entry->shared.len, f);
        fputc('\n', f);
        return fclose(f);
    }

    return IMAP_IOERROR;
}