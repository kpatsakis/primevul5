bool Regexp_processor_pcre::compile(String *pattern, bool send_error)
{
  const char *pcreErrorStr;
  int pcreErrorOffset;

  if (is_compiled())
  {
    if (!stringcmp(pattern, &m_prev_pattern))
      return false;
    cleanup();
    m_prev_pattern.copy(*pattern);
  }

  if (!(pattern= convert_if_needed(pattern, &pattern_converter)))
    return true;

  m_pcre= pcre_compile(pattern->c_ptr_safe(), m_library_flags,
                       &pcreErrorStr, &pcreErrorOffset, NULL);

  if (unlikely(m_pcre == NULL))
  {
    if (send_error)
    {
      char buff[MAX_FIELD_WIDTH];
      my_snprintf(buff, sizeof(buff), "%s at offset %d", pcreErrorStr, pcreErrorOffset);
      my_error(ER_REGEXP_ERROR, MYF(0), buff);
    }
    return true;
  }
  return false;
}