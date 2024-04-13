void Lex_input_stream::body_utf8_append_ident(THD *thd,
                                              const LEX_STRING *txt,
                                              const char *end_ptr)
{
  if (!m_cpp_utf8_processed_ptr)
    return;

  LEX_STRING utf_txt;
  CHARSET_INFO *txt_cs= thd->charset();

  if (!my_charset_same(txt_cs, &my_charset_utf8_general_ci))
  {
    thd->convert_string(&utf_txt,
                        &my_charset_utf8_general_ci,
                        txt->str, (uint) txt->length,
                        txt_cs);
  }
  else
  {
    utf_txt.str= txt->str;
    utf_txt.length= txt->length;
  }

  /* NOTE: utf_txt.length is in bytes, not in symbols. */

  memcpy(m_body_utf8_ptr, utf_txt.str, utf_txt.length);
  m_body_utf8_ptr += utf_txt.length;
  *m_body_utf8_ptr= 0;

  m_cpp_utf8_processed_ptr= end_ptr;
}