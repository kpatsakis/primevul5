static void output_entryatt(annotate_state_t *state, const char *entry,
                            const char *userid, const struct buf *value)
{
    const char *mboxname;
    char key[MAX_MAILBOX_PATH+1]; /* XXX MAX_MAILBOX_NAME + entry + userid */
    struct buf buf = BUF_INITIALIZER;

    /* We don't put any funny interpretations on NULL values for
     * some of these anymore, now that the dirty hacks are gone. */
    assert(state);
    assert(entry);
    assert(userid);
    assert(value);

    if (state->mailbox)
        mboxname = state->mailbox->name;
    else if (state->mbentry)
        mboxname = state->mbentry->name;
    else
        mboxname = "";
    /* @mboxname is now an internal mailbox name */

    /* Check if this is a new entry.
     * If so, flush our current entry.
     */
    if (state->uid != state->lastuid ||
        strcmp(mboxname, state->lastname) ||
        strcmp(entry, state->lastentry))
        flush_entryatt(state);

    strlcpy(state->lastname, mboxname, sizeof(state->lastname));
    strlcpy(state->lastentry, entry, sizeof(state->lastentry));
    state->lastuid = state->uid;

    /* check if we already returned this entry */
    strlcpy(key, mboxname, sizeof(key));
    if (state->uid) {
        char uidbuf[32];
        snprintf(uidbuf, sizeof(uidbuf), "/UID%u/", state->uid);
        strlcat(key, uidbuf, sizeof(key));
    }
    strlcat(key, entry, sizeof(key));
    strlcat(key, userid, sizeof(key));
    if (hash_lookup(key, &state->entry_table)) return;
    hash_insert(key, (void *)0xDEADBEEF, &state->entry_table);

    if (!userid[0]) { /* shared annotation */
        if ((state->attribs & ATTRIB_VALUE_SHARED)) {
            appendattvalue(&state->attvalues, "value.shared", value);
            state->found |= ATTRIB_VALUE_SHARED;
        }

        if ((state->attribs & ATTRIB_SIZE_SHARED)) {
            buf_reset(&buf);
            buf_printf(&buf, SIZE_T_FMT, value->len);
            appendattvalue(&state->attvalues, "size.shared", &buf);
            state->found |= ATTRIB_SIZE_SHARED;
        }
    }
    else { /* private annotation */
        if ((state->attribs & ATTRIB_VALUE_PRIV)) {
            appendattvalue(&state->attvalues, "value.priv", value);
            state->found |= ATTRIB_VALUE_PRIV;
        }

        if ((state->attribs & ATTRIB_SIZE_PRIV)) {
            buf_reset(&buf);
            buf_printf(&buf, SIZE_T_FMT, value->len);
            appendattvalue(&state->attvalues, "size.priv", &buf);
            state->found |= ATTRIB_SIZE_PRIV;
        }
    }
    buf_free(&buf);
}