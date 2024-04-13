static int imap_ac_add(struct Account *a, struct Mailbox *m)
{
  struct ImapAccountData *adata = a->adata;

  if (!adata)
  {
    struct ConnAccount cac = { { 0 } };
    char mailbox[PATH_MAX];

    if (imap_parse_path(mailbox_path(m), &cac, mailbox, sizeof(mailbox)) < 0)
      return -1;

    adata = imap_adata_new(a);
    adata->conn = mutt_conn_new(&cac);
    if (!adata->conn)
    {
      imap_adata_free((void **) &adata);
      return -1;
    }

    mutt_account_hook(m->realpath);

    if (imap_login(adata) < 0)
    {
      imap_adata_free((void **) &adata);
      return -1;
    }

    a->adata = adata;
    a->adata_free = imap_adata_free;
  }

  if (!m->mdata)
  {
    struct Url *url = url_parse(mailbox_path(m));
    struct ImapMboxData *mdata = imap_mdata_new(adata, url->path);

    /* fixup path and realpath, mainly to replace / by /INBOX */
    char buf[1024];
    imap_qualify_path(buf, sizeof(buf), &adata->conn->account, mdata->name);
    mutt_buffer_strcpy(&m->pathbuf, buf);
    mutt_str_replace(&m->realpath, mailbox_path(m));

    m->mdata = mdata;
    m->mdata_free = imap_mdata_free;
    url_free(&url);
  }
  return 0;
}