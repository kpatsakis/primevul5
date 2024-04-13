static void init_annotation_definitions(void)
{
    char *p;
    char aline[ANNOT_DEF_MAXLINELEN];
    annotate_entrydesc_t *ae;
    int i;
    FILE* f;
    struct parse_state state;
    ptrarray_t *entries = NULL;

    /* copy static entries into list */
    for (i = 0 ; server_builtin_entries[i].name ; i++)
        ptrarray_append(&server_entries, (void *)&server_builtin_entries[i]);

    /* copy static entries into list */
    for (i = 0 ; mailbox_builtin_entries[i].name ; i++)
        ptrarray_append(&mailbox_entries, (void *)&mailbox_builtin_entries[i]);

    /* copy static entries into list */
    for (i = 0 ; message_builtin_entries[i].name ; i++)
        ptrarray_append(&message_entries, (void *)&message_builtin_entries[i]);

    memset(&state, 0, sizeof(state));

    /* parse config file */
    state.filename = config_getstring(IMAPOPT_ANNOTATION_DEFINITIONS);

    if (!state.filename)
        return;

    f = fopen(state.filename,"r");
    if (!f) {
        syslog(LOG_ERR, "%s: could not open annotation definition file: %m",
               state.filename);
        return;
    }

    while (fgets(aline, sizeof(aline), f)) {
        /* remove leading space, skip blank lines and comments */
        state.lineno++;
        for (p = aline; *p && isspace(*p); p++);
        if (!*p || *p == '#') continue;
        tok_initm(&state.tok, aline, ",", TOK_TRIMLEFT|TOK_TRIMRIGHT|TOK_EMPTY);

        /* note, we only do the most basic validity checking and may
           be more restrictive than necessary */

        ae = xzmalloc(sizeof(*ae));

        if (!(p = get_token(&state, ".-_/:"))) goto bad;
        /* TV-TODO: should test for empty */

        if (!strncmp(p, IMAP_ANNOT_NS, strlen(IMAP_ANNOT_NS))) {
            parse_error(&state, "annotation under " IMAP_ANNOT_NS);
            goto bad;
        }
        ae->name = xstrdup(p);

        if (!(p = get_token(&state, ".-_/"))) goto bad;
        switch (table_lookup(annotation_scope_names, p)) {
        case ANNOTATION_SCOPE_SERVER:
            entries = &server_entries;
            break;
        case ANNOTATION_SCOPE_MAILBOX:
            entries = &mailbox_entries;
            break;
        case ANNOTATION_SCOPE_MESSAGE:
            if (!strncmp(ae->name, "/flags/", 7)) {
                /* RFC5257 reserves the /flags/ hierarchy for future use */
                state.context = ae->name;
                parse_error(&state, "message entry under /flags/");
                goto bad;
            }
            entries = &message_entries;
            break;
        case -1:
            parse_error(&state, "invalid annotation scope");
            goto bad;
        }

        if (!(p = get_token(&state, NULL))) goto bad;
        i = table_lookup(attribute_type_names, p);
        if (i < 0) {
            parse_error(&state, "invalid annotation type");
            goto bad;
        }
        ae->type = i;

        i = parse_table_lookup_bitmask(annotation_proxy_type_names, &state);
        if (i < 0) {
            parse_error(&state, "invalid annotation proxy type");
            goto bad;
        }
        ae->proxytype = i;

        i = parse_table_lookup_bitmask(annotation_attributes, &state);
        if (i < 0) {
            parse_error(&state, "invalid annotation attributes");
            goto bad;
        }
        ae->attribs = normalise_attribs(&state, i);

        if (!(p = get_token(&state, NULL))) goto bad;
        cyrus_acl_strtomask(p, &ae->extra_rights);
        /* XXX and if strtomask fails? */

        p = tok_next(&state.tok);
        if (p) {
            parse_error(&state, "junk at end of line");
            goto bad;
        }

        ae->get = annotation_get_fromdb;
        ae->set = annotation_set_todb;
        ae->rock = NULL;
        ptrarray_append(entries, ae);
        continue;

bad:
        free((char *)ae->name);
        free(ae);
        tok_fini(&state.tok);
        continue;
    }


#if 0
/* Suppress the syslog message to fix the unit tests, but have the
 * syslog message to aid the admin ...
 */
    if (state.nerrors)
        syslog(LOG_ERR, "%s: encountered %u errors.  Struggling on, but "
                        "some of your annotation definitions may be "
                        "ignored.  Please fix this file!",
                        state.filename, state.nerrors);
#endif

    fclose(f);
}