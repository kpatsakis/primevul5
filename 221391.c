static IMAP_COMMAND* cmd_new (IMAP_DATA* idata)
{
  IMAP_COMMAND* cmd;

  if (cmd_queue_full (idata))
  {
    dprint (3, (debugfile, "cmd_new: IMAP command queue full\n"));
    return NULL;
  }

  cmd = idata->cmds + idata->nextcmd;
  idata->nextcmd = (idata->nextcmd + 1) % idata->cmdslots;

  snprintf (cmd->seq, sizeof (cmd->seq), "a%04u", idata->seqno++);
  if (idata->seqno > 9999)
    idata->seqno = 0;

  cmd->state = IMAP_CMD_NEW;

  return cmd;
}