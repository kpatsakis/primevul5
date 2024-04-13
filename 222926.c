void imap_logout_all(void)
{
  struct Account *np = NULL;
  TAILQ_FOREACH(np, &NeoMutt->accounts, entries)
  {
    if (np->type != MUTT_IMAP)
      continue;

    struct ImapAccountData *adata = np->adata;
    if (!adata)
      continue;

    struct Connection *conn = adata->conn;
    if (!conn || (conn->fd < 0))
      continue;

    mutt_message(_("Closing connection to %s..."), conn->account.host);
    imap_logout(np->adata);
    mutt_clear_error();
  }
}