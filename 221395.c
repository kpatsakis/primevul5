const char* imap_cmd_trailer (IMAP_DATA* idata)
{
  static const char* notrailer = "";
  const char* s = idata->buf;

  if (!s)
  {
    dprint (2, (debugfile, "imap_cmd_trailer: not a tagged response"));
    return notrailer;
  }

  s = imap_next_word ((char *)s);
  if (!s || (ascii_strncasecmp (s, "OK", 2) &&
	     ascii_strncasecmp (s, "NO", 2) &&
	     ascii_strncasecmp (s, "BAD", 3)))
  {
    dprint (2, (debugfile, "imap_cmd_trailer: not a command completion: %s",
		idata->buf));
    return notrailer;
  }

  s = imap_next_word ((char *)s);
  if (!s)
    return notrailer;

  return s;
}