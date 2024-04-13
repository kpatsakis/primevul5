static int imap_mbox_open_append(struct Mailbox *m, OpenMailboxFlags flags)
{
  if (!m->account)
    return -1;

  /* in APPEND mode, we appear to hijack an existing IMAP connection -
   * ctx is brand new and mostly empty */
  struct ImapAccountData *adata = imap_adata_get(m);
  struct ImapMboxData *mdata = imap_mdata_get(m);

  int rc = imap_mailbox_status(m, false);
  if (rc >= 0)
    return 0;
  if (rc == -1)
    return -1;

  char buf[PATH_MAX + 64];
  snprintf(buf, sizeof(buf), _("Create %s?"), mdata->name);
  if (C_Confirmcreate && (mutt_yesorno(buf, MUTT_YES) != MUTT_YES))
    return -1;

  if (imap_create_mailbox(adata, mdata->name) < 0)
    return -1;

  return 0;
}