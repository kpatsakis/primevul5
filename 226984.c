void imap_qualify_path(char *buf, size_t buflen, struct ConnAccount *cac, char *path)
{
  struct Url url = { 0 };
  mutt_account_tourl(cac, &url);
  url.path = path;
  url_tostring(&url, buf, buflen, U_NO_FLAGS);
}