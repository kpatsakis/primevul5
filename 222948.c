static int imap_path_pretty(char *buf, size_t buflen, const char *folder)
{
  if (!folder)
    return -1;

  imap_pretty_mailbox(buf, buflen, folder);
  return 0;
}