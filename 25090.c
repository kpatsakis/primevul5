String *Regexp_processor_pcre::convert_if_needed(String *str, String *converter)
{
  if (m_conversion_is_needed)
  {
    uint dummy_errors;
    if (converter->copy(str->ptr(), str->length(), str->charset(),
                        m_library_charset, &dummy_errors))
      return NULL;
    str= converter;
  }
  return str;
}