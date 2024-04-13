static int imap_tags_commit(struct Mailbox *m, struct Email *e, char *buf)
{
  char uid[11];

  struct ImapAccountData *adata = imap_adata_get(m);

  if (*buf == '\0')
    buf = NULL;

  if (!(adata->mailbox->rights & MUTT_ACL_WRITE))
    return 0;

  snprintf(uid, sizeof(uid), "%u", imap_edata_get(e)->uid);

  /* Remove old custom flags */
  if (imap_edata_get(e)->flags_remote)
  {
    struct Buffer cmd = mutt_buffer_make(128); // just a guess
    mutt_buffer_addstr(&cmd, "UID STORE ");
    mutt_buffer_addstr(&cmd, uid);
    mutt_buffer_addstr(&cmd, " -FLAGS.SILENT (");
    mutt_buffer_addstr(&cmd, imap_edata_get(e)->flags_remote);
    mutt_buffer_addstr(&cmd, ")");

    /* Should we return here, or we are fine and we could
     * continue to add new flags */
    int rc = imap_exec(adata, cmd.data, IMAP_CMD_NO_FLAGS);
    mutt_buffer_dealloc(&cmd);
    if (rc != IMAP_EXEC_SUCCESS)
    {
      return -1;
    }
  }

  /* Add new custom flags */
  if (buf)
  {
    struct Buffer cmd = mutt_buffer_make(128); // just a guess
    mutt_buffer_addstr(&cmd, "UID STORE ");
    mutt_buffer_addstr(&cmd, uid);
    mutt_buffer_addstr(&cmd, " +FLAGS.SILENT (");
    mutt_buffer_addstr(&cmd, buf);
    mutt_buffer_addstr(&cmd, ")");

    int rc = imap_exec(adata, cmd.data, IMAP_CMD_NO_FLAGS);
    mutt_buffer_dealloc(&cmd);
    if (rc != IMAP_EXEC_SUCCESS)
    {
      mutt_debug(LL_DEBUG1, "fail to add new flags\n");
      return -1;
    }
  }

  /* We are good sync them */
  mutt_debug(LL_DEBUG1, "NEW TAGS: %s\n", buf);
  driver_tags_replace(&e->tags, buf);
  FREE(&imap_edata_get(e)->flags_remote);
  imap_edata_get(e)->flags_remote = driver_tags_get_with_hidden(&e->tags);
  return 0;
}