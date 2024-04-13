int imap_delete_mailbox(struct Mailbox *m, char *path)
{
  char buf[PATH_MAX + 7];
  char mbox[PATH_MAX];
  struct Url *url = url_parse(path);

  struct ImapAccountData *adata = imap_adata_get(m);
  imap_munge_mbox_name(adata->unicode, mbox, sizeof(mbox), url->path);
  url_free(&url);
  snprintf(buf, sizeof(buf), "DELETE %s", mbox);
  if (imap_exec(m->account->adata, buf, IMAP_CMD_NO_FLAGS) != IMAP_EXEC_SUCCESS)
    return -1;

  return 0;
}