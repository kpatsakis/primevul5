void imap_close_connection(struct ImapAccountData *adata)
{
  if (adata->state != IMAP_DISCONNECTED)
  {
    mutt_socket_close(adata->conn);
    adata->state = IMAP_DISCONNECTED;
  }
  adata->seqno = 0;
  adata->nextcmd = 0;
  adata->lastcmd = 0;
  adata->status = 0;
  memset(adata->cmds, 0, sizeof(struct ImapCommand) * adata->cmdslots);
}