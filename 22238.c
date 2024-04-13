static int annotation_set_pop3showafter(annotate_state_t *state,
                                        struct annotate_entry_list *entry,
                                        int maywrite)
{
    struct mailbox *mailbox = state->mailbox;
    int r = 0;
    time_t date;

    assert(mailbox);

    if (entry->shared.s == NULL) {
        /* Effectively removes the annotation */
        date = 0;
    }
    else {
        r = time_from_rfc5322(buf_cstring(&entry->shared), &date, DATETIME_FULL);
        if (r < 0)
            return IMAP_PROTOCOL_BAD_PARAMETERS;
    }

    if (date != mailbox->i.pop3_show_after) {
        if (!maywrite) return IMAP_PERMISSION_DENIED;
        mailbox_index_dirty(mailbox);
        mailbox_modseq_dirty(mailbox);
        mailbox->i.pop3_show_after = date;
        mboxlist_update_foldermodseq(mailbox->name, mailbox->i.highestmodseq);
    }

    return 0;
}