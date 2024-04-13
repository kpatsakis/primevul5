int my_wc_mb_utf8_opt_escape(CHARSET_INFO *cs,
                             my_wc_t wc, my_wc_t escape, my_wc_t ewc,
                             uchar *str, uchar *end)
{
  return escape ? my_wc_mb_utf8_with_escape(cs, escape, ewc, str, end) :
                  my_charset_utf8_handler.wc_mb(cs, wc, str, end);
}