void imap_notify_delete_email(struct Mailbox *m, struct Email *e)
{
  struct ImapMboxData *mdata = imap_mdata_get(m);
  struct ImapEmailData *edata = imap_edata_get(e);

  if (!mdata || !edata)
    return;

  imap_msn_remove(&mdata->msn, edata->msn - 1);
  edata->msn = 0;
}