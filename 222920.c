int imap_check_mailbox(struct Mailbox *m, bool force)
{
  if (!m || !m->account)
    return -1;

  struct ImapAccountData *adata = imap_adata_get(m);
  struct ImapMboxData *mdata = imap_mdata_get(m);

  /* overload keyboard timeout to avoid many mailbox checks in a row.
   * Most users don't like having to wait exactly when they press a key. */
  int rc = 0;

  /* try IDLE first, unless force is set */
  if (!force && C_ImapIdle && (adata->capabilities & IMAP_CAP_IDLE) &&
      ((adata->state != IMAP_IDLE) || (mutt_date_epoch() >= adata->lastread + C_ImapKeepalive)))
  {
    if (imap_cmd_idle(adata) < 0)
      return -1;
  }
  if (adata->state == IMAP_IDLE)
  {
    while ((rc = mutt_socket_poll(adata->conn, 0)) > 0)
    {
      if (imap_cmd_step(adata) != IMAP_RES_CONTINUE)
      {
        mutt_debug(LL_DEBUG1, "Error reading IDLE response\n");
        return -1;
      }
    }
    if (rc < 0)
    {
      mutt_debug(LL_DEBUG1, "Poll failed, disabling IDLE\n");
      adata->capabilities &= ~IMAP_CAP_IDLE; // Clear the flag
    }
  }

  if ((force || ((adata->state != IMAP_IDLE) &&
                 (mutt_date_epoch() >= adata->lastread + C_Timeout))) &&
      (imap_exec(adata, "NOOP", IMAP_CMD_POLL) != IMAP_EXEC_SUCCESS))
  {
    return -1;
  }

  /* We call this even when we haven't run NOOP in case we have pending
   * changes to process, since we can reopen here. */
  imap_cmd_finish(adata);

  if (mdata->check_status & IMAP_EXPUNGE_PENDING)
    rc = MUTT_REOPENED;
  else if (mdata->check_status & IMAP_NEWMAIL_PENDING)
    rc = MUTT_NEW_MAIL;
  else if (mdata->check_status & IMAP_FLAGS_PENDING)
    rc = MUTT_FLAGS;

  mdata->check_status = IMAP_OPEN_NO_FLAGS;

  return rc;
}