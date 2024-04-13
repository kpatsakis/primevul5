int imap_mailbox_status(struct Mailbox *m, bool queue)
{
  struct ImapAccountData *adata = imap_adata_get(m);
  struct ImapMboxData *mdata = imap_mdata_get(m);
  if (!adata || !mdata)
    return -1;
  return imap_status(adata, mdata, queue);
}