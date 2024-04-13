void imap_disallow_reopen(struct Mailbox *m)
{
  struct ImapAccountData *adata = imap_adata_get(m);
  struct ImapMboxData *mdata = imap_mdata_get(m);
  if (!adata || !adata->mailbox || (adata->mailbox != m) || !mdata)
    return;
  mdata->reopen &= ~IMAP_REOPEN_ALLOW;
}