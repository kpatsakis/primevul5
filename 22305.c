static int find_desc_store(annotate_state_t *state,
                           const char *name,
                           const annotate_entrydesc_t **descp)
{
    int scope = state->which;
    const ptrarray_t *descs;
    const annotate_entrydesc_t *db_entry;
    annotate_entrydesc_t *desc;
    int i;

    if (scope == ANNOTATION_SCOPE_SERVER) {
        descs = &server_entries;
        db_entry = &server_db_entry;
    }
    else if (scope == ANNOTATION_SCOPE_MAILBOX) {
        descs = &mailbox_entries;
        db_entry = &mailbox_db_entry;
    }
    else if (scope == ANNOTATION_SCOPE_MESSAGE) {
        descs = &message_entries;
        db_entry = &message_db_entry;
    }
    else {
        syslog(LOG_ERR, "IOERROR: unknown scope in find_desc_store %d", scope);
        return IMAP_INTERNAL;
    }

    /* check for DAV annotations */
    if (state->mailbox && (state->mailbox->mbtype & MBTYPES_DAV) &&
        !strncmp(name, DAV_ANNOT_NS, strlen(DAV_ANNOT_NS))) {
        *descp = db_entry;
        return 0;
    }

    /* check known IMAP annotations */
    for (i = 0 ; i < descs->count ; i++) {
        desc = descs->data[i];
        if (strcmp(name, desc->name))
            continue;
        if (!desc->set) {
            /* read-only annotation */
            return IMAP_PERMISSION_DENIED;
        }
        *descp = desc;
        return 0;
    }

    /* unknown annotation */
    if (!config_getswitch(IMAPOPT_ANNOTATION_ALLOW_UNDEFINED))
        return IMAP_PERMISSION_DENIED;

    /* check for /flags and /vendor/cmu */
    if (scope == ANNOTATION_SCOPE_MESSAGE &&
        !strncmp(name, "/flags/", 7))
        return IMAP_PERMISSION_DENIED;

    if (!strncmp(name, IMAP_ANNOT_NS, strlen(IMAP_ANNOT_NS)))
        return IMAP_PERMISSION_DENIED;

    *descp = db_entry;
    return 0;
}