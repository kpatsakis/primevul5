static int annotation_set_mailboxopt(annotate_state_t *state,
                                     struct annotate_entry_list *entry,
                                     int maywrite)
{
    struct mailbox *mailbox = state->mailbox;
    uint32_t flag = (unsigned long)entry->desc->rock;
    unsigned long newopts;

    assert(mailbox);

    newopts = mailbox->i.options;

    if (entry->shared.s &&
        !strcmp(entry->shared.s, "true")) {
        newopts |= flag;
    } else {
        newopts &= ~flag;
    }

    /* only mark dirty if there's been a change */
    if (mailbox->i.options != newopts) {
        if (!maywrite) return IMAP_PERMISSION_DENIED;
        mailbox_index_dirty(mailbox);
        mailbox_modseq_dirty(mailbox);
        mailbox->i.options = newopts;
        mboxlist_update_foldermodseq(mailbox->name, mailbox->i.highestmodseq);
    }

    return 0;
}