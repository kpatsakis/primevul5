void imap_get_parent(const char *mbox, char delim, char *buf, size_t buflen)
{
  /* Make a copy of the mailbox name, but only if the pointers are different */
  if (mbox != buf)
    mutt_str_copy(buf, mbox, buflen);

  int n = mutt_str_len(buf);

  /* Let's go backwards until the next delimiter
   *
   * If buf[n] is a '/', the first n-- will allow us
   * to ignore it. If it isn't, then buf looks like
   * "/aaaaa/bbbb". There is at least one "b", so we can't skip
   * the "/" after the 'a's.
   *
   * If buf == '/', then n-- => n == 0, so the loop ends
   * immediately */
  for (n--; (n >= 0) && (buf[n] != delim); n--)
    ; // do nothing

  /* We stopped before the beginning. There is a trailing slash.  */
  if (n > 0)
  {
    /* Strip the trailing delimiter.  */
    buf[n] = '\0';
  }
  else
  {
    buf[0] = (n == 0) ? delim : '\0';
  }
}