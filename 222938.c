static int imap_status(struct ImapAccountData *adata, struct ImapMboxData *mdata, bool queue)
{
  char *uidvalidity_flag = NULL;
  char cmd[2048];

  if (!adata || !mdata)
    return -1;

  /* Don't issue STATUS on the selected mailbox, it will be NOOPed or
   * IDLEd elsewhere.
   * adata->mailbox may be NULL for connections other than the current
   * mailbox's. */
  if (adata->mailbox && (adata->mailbox->mdata == mdata))
  {
    adata->mailbox->has_new = false;
    return mdata->messages;
  }

  if (adata->capabilities & IMAP_CAP_IMAP4REV1)
    uidvalidity_flag = "UIDVALIDITY";
  else if (adata->capabilities & IMAP_CAP_STATUS)
    uidvalidity_flag = "UID-VALIDITY";
  else
  {
    mutt_debug(LL_DEBUG2, "Server doesn't support STATUS\n");
    return -1;
  }

  snprintf(cmd, sizeof(cmd), "STATUS %s (UIDNEXT %s UNSEEN RECENT MESSAGES)",
           mdata->munge_name, uidvalidity_flag);

  int rc = imap_exec(adata, cmd, queue ? IMAP_CMD_QUEUE : IMAP_CMD_NO_FLAGS | IMAP_CMD_POLL);
  if (rc < 0)
  {
    mutt_debug(LL_DEBUG1, "Error queueing command\n");
    return rc;
  }
  return mdata->messages;
}