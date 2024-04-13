enum MailboxType imap_path_probe(const char *path, const struct stat *st)
{
  if (mutt_istr_startswith(path, "imap://"))
    return MUTT_IMAP;

  if (mutt_istr_startswith(path, "imaps://"))
    return MUTT_IMAP;

  return MUTT_UNKNOWN;
}