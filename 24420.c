Lex_input_stream::get_escape_func(THD *thd, my_wc_t sep) const
{
  return thd->backslash_escapes() ?
         (sep == '"' ? my_wc_mb_utf8_escape_double_quote_and_backslash:
                       my_wc_mb_utf8_escape_single_quote_and_backslash) :
         (sep == '"' ? my_wc_mb_utf8_escape_double_quote:
                       my_wc_mb_utf8_escape_single_quote);
}