int imap_exec (IMAP_DATA* idata, const char* cmdstr, int flags)
{
  int rc;

  if ((rc = cmd_start (idata, cmdstr, flags)) < 0)
  {
    cmd_handle_fatal (idata);
    return -1;
  }

  if (flags & IMAP_CMD_QUEUE)
    return 0;

  if ((flags & IMAP_CMD_POLL) &&
      (ImapPollTimeout > 0) &&
      (mutt_socket_poll (idata->conn, ImapPollTimeout)) == 0)
  {
    mutt_error (_("Connection to %s timed out"), idata->conn->account.host);
    mutt_sleep (2);
    cmd_handle_fatal (idata);
    return -1;
  }

  do
    rc = imap_cmd_step (idata);
  while (rc == IMAP_CMD_CONTINUE);

  if (rc == IMAP_CMD_NO && (flags & IMAP_CMD_FAIL_OK))
    return -2;

  if (rc != IMAP_CMD_OK)
  {
    if ((flags & IMAP_CMD_FAIL_OK) && idata->status != IMAP_FATAL)
      return -2;

    dprint (1, (debugfile, "imap_exec: command failed: %s\n", idata->buf));
    return -1;
  }

  return 0;
}