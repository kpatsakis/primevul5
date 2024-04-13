static struct Account *imap_ac_find(struct Account *a, const char *path)
{
  struct Url *url = url_parse(path);
  if (!url)
    return NULL;

  struct ImapAccountData *adata = a->adata;
  struct ConnAccount *cac = &adata->conn->account;

  if (!mutt_istr_equal(url->host, cac->host))
    a = NULL;
  else if (url->user && !mutt_istr_equal(url->user, cac->user))
    a = NULL;

  url_free(&url);
  return a;
}