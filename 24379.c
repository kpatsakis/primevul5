uint Lex_input_stream::get_body_utf8_maximum_length(THD *thd)
{
  /*
    String literals can grow during escaping:
    1a. Character string '<TAB>' can grow to '\t', 3 bytes to 4 bytes growth.
    1b. Character string '1000 times <TAB>' grows from
        1002 to 2002 bytes (including quotes), which gives a little bit
        less than 2 times growth.
    "2" should be a reasonable multiplier that safely covers escaping needs.
  */
  return (m_buf_length / thd->variables.character_set_client->mbminlen) *
          my_charset_utf8_bin.mbmaxlen * 2/*for escaping*/;
}