void imap_keepalive(void)
{
  time_t now = mutt_date_epoch();
  struct Account *np = NULL;
  TAILQ_FOREACH(np, &NeoMutt->accounts, entries)
  {
    if (np->type != MUTT_IMAP)
      continue;

    struct ImapAccountData *adata = np->adata;
    if (!adata || !adata->mailbox)
      continue;

    const short c_imap_keepalive =
        cs_subset_number(NeoMutt->sub, "imap_keepalive");
    if ((adata->state >= IMAP_AUTHENTICATED) && (now >= (adata->lastread + c_imap_keepalive)))
      imap_check_mailbox(adata->mailbox, true);
  }
}