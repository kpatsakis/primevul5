int imap_adata_find(const char *path, struct ImapAccountData **adata,
                    struct ImapMboxData **mdata)
{
  struct ConnAccount cac = { { 0 } };
  struct ImapAccountData *tmp_adata = NULL;
  char tmp[1024];

  if (imap_parse_path(path, &cac, tmp, sizeof(tmp)) < 0)
    return -1;

  struct Account *np = NULL;
  TAILQ_FOREACH(np, &NeoMutt->accounts, entries)
  {
    if (np->type != MUTT_IMAP)
      continue;

    tmp_adata = np->adata;
    if (!tmp_adata)
      continue;
    if (imap_account_match(&tmp_adata->conn->account, &cac))
    {
      *mdata = imap_mdata_new(tmp_adata, tmp);
      *adata = tmp_adata;
      return 0;
    }
  }
  mutt_debug(LL_DEBUG3, "no ImapAccountData found\n");
  return -1;
}