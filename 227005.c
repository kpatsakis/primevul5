void imap_clean_path(char *path, size_t plen)
{
  struct ImapAccountData *adata = NULL;
  struct ImapMboxData *mdata = NULL;

  if (imap_adata_find(path, &adata, &mdata) < 0)
    return;

  /* Returns a fully qualified IMAP url */
  imap_qualify_path(path, plen, &adata->conn->account, mdata->name);
  imap_mdata_free((void *) &mdata);
}