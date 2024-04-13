void imap_munge_mbox_name(bool unicode, char *dest, size_t dlen, const char *src)
{
  char *buf = mutt_str_dup(src);
  imap_utf_encode(unicode, &buf);

  imap_quote_string(dest, dlen, buf, false);

  FREE(&buf);
}