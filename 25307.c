bool Regexp_processor_pcre::exec(const char *str, size_t length, size_t offset)
{
  m_pcre_exec_rc= pcre_exec_with_warn(m_pcre, &m_pcre_extra, str, (int)length, (int)offset, 0,
                                      m_SubStrVec, array_elements(m_SubStrVec));
  return false;
}