static int imap_path_is_empty(const char *path)
{
  int rc = imap_path_status(path, false);
  if (rc < 0)
    return -1;
  if (rc == 0)
    return 1;
  return 0;
}