static int cmd_queue (IMAP_DATA* idata, const char* cmdstr, int flags)
{
  IMAP_COMMAND* cmd;
  int rc;

  if (cmd_queue_full (idata))
  {
    dprint (3, (debugfile, "Draining IMAP command pipeline\n"));

    rc = imap_exec (idata, NULL, IMAP_CMD_FAIL_OK | (flags & IMAP_CMD_POLL));

    if (rc < 0 && rc != -2)
      return rc;
  }

  if (!(cmd = cmd_new (idata)))
    return IMAP_CMD_BAD;

  if (mutt_buffer_printf (idata->cmdbuf, "%s %s\r\n", cmd->seq, cmdstr) < 0)
    return IMAP_CMD_BAD;

  return 0;
}