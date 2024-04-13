static char *get_flags(struct ListHead *hflags, char *s)
{
  /* sanity-check string */
  const size_t plen = mutt_istr_startswith(s, "FLAGS");
  if (plen == 0)
  {
    mutt_debug(LL_DEBUG1, "not a FLAGS response: %s\n", s);
    return NULL;
  }
  s += plen;
  SKIPWS(s);
  if (*s != '(')
  {
    mutt_debug(LL_DEBUG1, "bogus FLAGS response: %s\n", s);
    return NULL;
  }

  /* update caller's flags handle */
  while (*s && (*s != ')'))
  {
    s++;
    SKIPWS(s);
    const char *flag_word = s;
    while (*s && (*s != ')') && !IS_SPACE(*s))
      s++;
    const char ctmp = *s;
    *s = '\0';
    if (*flag_word)
      mutt_list_insert_tail(hflags, mutt_str_dup(flag_word));
    *s = ctmp;
  }

  /* note bad flags response */
  if (*s != ')')
  {
    mutt_debug(LL_DEBUG1, "Unterminated FLAGS response: %s\n", s);
    mutt_list_free(hflags);

    return NULL;
  }

  s++;

  return s;
}