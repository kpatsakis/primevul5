int my_wc_mb_utf8_escape_single_quote(CHARSET_INFO *cs, my_wc_t wc,
                                      uchar *str, uchar *end)
{
  return my_wc_mb_utf8_escape(cs, wc, str, end, '\'', 0);
}