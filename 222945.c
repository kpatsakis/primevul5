static void imap_mbox_select(struct Mailbox *m)
{
  struct ImapAccountData *adata = imap_adata_get(m);
  struct ImapMboxData *mdata = imap_mdata_get(m);
  if (!adata || !mdata)
    return;

  const char *condstore = NULL;
#ifdef USE_HCACHE
  if ((adata->capabilities & IMAP_CAP_CONDSTORE) && C_ImapCondstore)
    condstore = " (CONDSTORE)";
  else
#endif
    condstore = "";

  char buf[PATH_MAX];
  snprintf(buf, sizeof(buf), "%s %s%s", m->readonly ? "EXAMINE" : "SELECT",
           mdata->munge_name, condstore);

  adata->state = IMAP_SELECTED;

  imap_cmd_start(adata, buf);
}