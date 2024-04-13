int my_wc_mb_utf8_escape(CHARSET_INFO *cs, my_wc_t wc, uchar *str, uchar *end,
                         my_wc_t sep, my_wc_t escape)
{
  DBUG_ASSERT(escape == 0 || escape == '\\');
  DBUG_ASSERT(sep == '"' || sep == '\'');
  switch (wc) {
  case 0:      return my_wc_mb_utf8_opt_escape(cs, wc, escape, '0', str, end);
  case '\t':   return my_wc_mb_utf8_opt_escape(cs, wc, escape, 't', str, end);
  case '\r':   return my_wc_mb_utf8_opt_escape(cs, wc, escape, 'r', str, end);
  case '\n':   return my_wc_mb_utf8_opt_escape(cs, wc, escape, 'n', str, end);
  case '\032': return my_wc_mb_utf8_opt_escape(cs, wc, escape, 'Z', str, end);
  case '\'':
  case '\"':
    if (wc == sep)
      return my_wc_mb_utf8_with_escape(cs, wc, wc, str, end);
  }
  return my_charset_utf8_handler.wc_mb(cs, wc, str, end); // No escaping needed
}