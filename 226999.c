void imap_unmunge_mbox_name(bool unicode, char *s)
{
  imap_unquote_string(s);

  char *buf = mutt_str_dup(s);
  if (buf)
  {
    imap_utf_decode(unicode, &buf);
    strncpy(s, buf, strlen(s));
  }

  FREE(&buf);
}