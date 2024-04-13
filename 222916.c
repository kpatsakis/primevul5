int imap_path_canon(char *buf, size_t buflen)
{
  struct Url *url = url_parse(buf);
  if (!url)
    return 0;

  char tmp[PATH_MAX];
  char tmp2[PATH_MAX];

  imap_fix_path('\0', url->path, tmp, sizeof(tmp));
  url->path = tmp;
  url_tostring(url, tmp2, sizeof(tmp2), 0);
  mutt_str_copy(buf, tmp2, buflen);
  url_free(&url);

  return 0;
}