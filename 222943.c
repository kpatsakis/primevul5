int imap_access(const char *path)
{
  if (imap_path_status(path, false) >= 0)
    return 0;
  return -1;
}