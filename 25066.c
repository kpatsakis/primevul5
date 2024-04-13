bool Regexp_processor_pcre::exec(String *str, int offset,
                                  uint n_result_offsets_to_convert)
{
  if (!(str= convert_if_needed(str, &subject_converter)))
    return true;
  m_pcre_exec_rc= pcre_exec_with_warn(m_pcre, &m_pcre_extra,
                                      str->c_ptr_safe(), str->length(),
                                      offset, 0,
                                      m_SubStrVec, array_elements(m_SubStrVec));
  if (m_pcre_exec_rc > 0)
  {
    uint i;
    for (i= 0; i < n_result_offsets_to_convert; i++)
    {
      /*
        Convert byte offset into character offset.
      */
      m_SubStrVec[i]= (int) str->charset()->cset->numchars(str->charset(),
                                                           str->ptr(),
                                                           str->ptr() +
                                                           m_SubStrVec[i]);
    }
  }
  return false;
}