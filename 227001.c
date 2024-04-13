void imap_cachepath(char delim, const char *mailbox, struct Buffer *dest)
{
  const char *p = mailbox;
  mutt_buffer_reset(dest);
  if (!p)
    return;

  while (*p)
  {
    if (p[0] == delim)
    {
      mutt_buffer_addch(dest, '/');
      /* simple way to avoid collisions with UIDs */
      if ((p[1] >= '0') && (p[1] <= '9'))
        mutt_buffer_addch(dest, '_');
    }
    else
      mutt_buffer_addch(dest, *p);
    p++;
  }
}