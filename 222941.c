static int imap_path_parent(char *buf, size_t buflen)
{
  char tmp[PATH_MAX] = { 0 };

  imap_get_parent_path(buf, tmp, sizeof(tmp));
  mutt_str_copy(buf, tmp, buflen);
  return 0;
}