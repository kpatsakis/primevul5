int imap_cmd_step (IMAP_DATA* idata)
{
  size_t len = 0;
  int c;
  int rc;
  int stillrunning = 0;
  IMAP_COMMAND* cmd;

  if (idata->status == IMAP_FATAL)
  {
    cmd_handle_fatal (idata);
    return IMAP_CMD_BAD;
  }

  /* read into buffer, expanding buffer as necessary until we have a full
   * line */
  do
  {
    if (len == idata->blen)
    {
      safe_realloc (&idata->buf, idata->blen + IMAP_CMD_BUFSIZE);
      idata->blen = idata->blen + IMAP_CMD_BUFSIZE;
      dprint (3, (debugfile, "imap_cmd_step: grew buffer to %u bytes\n",
		  idata->blen));
    }

    /* back up over '\0' */
    if (len)
      len--;
    c = mutt_socket_readln (idata->buf + len, idata->blen - len, idata->conn);
    if (c <= 0)
    {
      dprint (1, (debugfile, "imap_cmd_step: Error reading server response.\n"));
      cmd_handle_fatal (idata);
      return IMAP_CMD_BAD;
    }

    len += c;
  }
  /* if we've read all the way to the end of the buffer, we haven't read a
   * full line (mutt_socket_readln strips the \r, so we always have at least
   * one character free when we've read a full line) */
  while (len == idata->blen);

  /* don't let one large string make cmd->buf hog memory forever */
  if ((idata->blen > IMAP_CMD_BUFSIZE) && (len <= IMAP_CMD_BUFSIZE))
  {
    safe_realloc (&idata->buf, IMAP_CMD_BUFSIZE);
    idata->blen = IMAP_CMD_BUFSIZE;
    dprint (3, (debugfile, "imap_cmd_step: shrank buffer to %u bytes\n", idata->blen));
  }

  idata->lastread = time (NULL);

  /* handle untagged messages. The caller still gets its shot afterwards. */
  if ((!ascii_strncmp (idata->buf, "* ", 2)
       || !ascii_strncmp (imap_next_word (idata->buf), "OK [", 4))
      && cmd_handle_untagged (idata))
    return IMAP_CMD_BAD;

  /* server demands a continuation response from us */
  if (idata->buf[0] == '+')
    return IMAP_CMD_RESPOND;

  /* Look for tagged command completions.
   *
   * Some response handlers can end up recursively calling
   * imap_cmd_step() and end up handling all tagged command
   * completions.
   * (e.g. FETCH->set_flag->set_header_color->~h pattern match.)
   *
   * Other callers don't even create an idata->cmds entry.
   *
   * For both these cases, we default to returning OK */
  rc = IMAP_CMD_OK;
  c = idata->lastcmd;
  do
  {
    cmd = &idata->cmds[c];
    if (cmd->state == IMAP_CMD_NEW)
    {
      if (!ascii_strncmp (idata->buf, cmd->seq, SEQLEN)) {
	if (!stillrunning)
	{
	  /* first command in queue has finished - move queue pointer up */
	  idata->lastcmd = (idata->lastcmd + 1) % idata->cmdslots;
	}
	cmd->state = cmd_status (idata->buf);
	/* bogus - we don't know which command result to return here. Caller
	 * should provide a tag. */
	rc = cmd->state;
      }
      else
	stillrunning++;
    }

    c = (c + 1) % idata->cmdslots;
  }
  while (c != idata->nextcmd);

  if (stillrunning)
    rc = IMAP_CMD_CONTINUE;
  else
  {
    dprint (3, (debugfile, "IMAP queue drained\n"));
    imap_cmd_finish (idata);
  }
  

  return rc;
}