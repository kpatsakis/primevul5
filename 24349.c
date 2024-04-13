Lex_input_stream::unescape(CHARSET_INFO *cs, char *to,
                           const char *str, const char *end,
                           int sep)
{
  return my_unescape(cs, to, str, end, sep, m_thd->backslash_escapes());
}