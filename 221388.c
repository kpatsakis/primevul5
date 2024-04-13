static int cmd_start (IMAP_DATA* idata, const char* cmdstr, int flags)
{
  int rc;

  if (idata->status == IMAP_FATAL)
  {
    cmd_handle_fatal (idata);
    return -1;
  }

  if (cmdstr && ((rc = cmd_queue (idata, cmdstr, flags)) < 0))
    return rc;

  if (flags & IMAP_CMD_QUEUE)
    return 0;

  if (idata->cmdbuf->dptr == idata->cmdbuf->data)
    return IMAP_CMD_BAD;

  rc = mutt_socket_write_d (idata->conn, idata->cmdbuf->data, -1,
                            flags & IMAP_CMD_PASS ? IMAP_LOG_PASS : IMAP_LOG_CMD);
  idata->cmdbuf->dptr = idata->cmdbuf->data;

  /* unidle when command queue is flushed */
  if (idata->state == IMAP_IDLE)
    idata->state = IMAP_SELECTED;

  return (rc < 0) ? IMAP_CMD_BAD : 0;
}