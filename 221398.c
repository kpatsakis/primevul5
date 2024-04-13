static int cmd_queue_full (IMAP_DATA* idata)
{
  if ((idata->nextcmd + 1) % idata->cmdslots == idata->lastcmd)
    return 1;

  return 0;
}