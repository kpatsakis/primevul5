char *imap_fix_path(char delim, const char *mailbox, char *path, size_t plen)
{
  int i = 0;
  for (; mailbox && *mailbox && (i < plen - 1); i++)
  {
    const char *const c_imap_delim_chars =
        cs_subset_string(NeoMutt->sub, "imap_delim_chars");
    if (*mailbox == delim || (!delim && strchr(NONULL(c_imap_delim_chars), *mailbox)))
    {
      delim = *mailbox;
      /* Skip multiple occurrences of delim */
      while (*mailbox && *(mailbox + 1) == delim)
        mailbox++;
    }
    path[i] = *mailbox++;
  }

  /* Do not terminate with a delimiter */
  if (i && path[i - 1] == delim)
    i--;

  /* Ensure null termination */
  path[i] = '\0';
  return path;
}