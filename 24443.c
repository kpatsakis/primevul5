int my_wc_mb_utf8_escape_double_quote_and_backslash(CHARSET_INFO *cs,
                                                    my_wc_t wc,
                                                    uchar *str, uchar *end)
{
  return my_wc_mb_utf8_escape(cs, wc, str, end, '"', '\\');
}