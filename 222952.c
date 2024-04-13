static int imap_mbox_close(struct Mailbox *m)
{
  struct ImapAccountData *adata = imap_adata_get(m);
  struct ImapMboxData *mdata = imap_mdata_get(m);

  /* Check to see if the mailbox is actually open */
  if (!adata || !mdata)
    return 0;

  /* imap_mbox_open_append() borrows the struct ImapAccountData temporarily,
   * just for the connection.
   *
   * So when these are equal, it means we are actually closing the
   * mailbox and should clean up adata.  Otherwise, we don't want to
   * touch adata - it's still being used.  */
  if (m == adata->mailbox)
  {
    if ((adata->status != IMAP_FATAL) && (adata->state >= IMAP_SELECTED))
    {
      /* mx_mbox_close won't sync if there are no deleted messages
       * and the mailbox is unchanged, so we may have to close here */
      if (m->msg_deleted == 0)
      {
        adata->closing = true;
        imap_exec(adata, "CLOSE", IMAP_CMD_QUEUE);
      }
      adata->state = IMAP_AUTHENTICATED;
    }

    mutt_debug(LL_DEBUG3, "closing %s, restoring %s\n", m->pathbuf.data,
               (adata->prev_mailbox ? adata->prev_mailbox->pathbuf.data : "(none)"));
    adata->mailbox = adata->prev_mailbox;
    imap_mbox_select(adata->prev_mailbox);
    imap_mdata_cache_reset(m->mdata);
  }

  return 0;
}