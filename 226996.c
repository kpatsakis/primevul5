char *imap_next_word(char *s)
{
  bool quoted = false;

  while (*s)
  {
    if (*s == '\\')
    {
      s++;
      if (*s)
        s++;
      continue;
    }
    if (*s == '\"')
      quoted = !quoted;
    if (!quoted && IS_SPACE(*s))
      break;
    s++;
  }

  SKIPWS(s);
  return s;
}