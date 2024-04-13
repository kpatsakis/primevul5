int Regexp_processor_pcre::pcre_exec_with_warn(const pcre *code,
                                               const pcre_extra *extra,
                                               const char *subject,
                                               int length, int startoffset,
                                               int options, int *ovector,
                                               int ovecsize)
{
  int rc= pcre_exec(code, extra, subject, length,
                    startoffset, options, ovector, ovecsize);
  DBUG_EXECUTE_IF("pcre_exec_error_123", rc= -123;);
  if (unlikely(rc < PCRE_ERROR_NOMATCH))
    pcre_exec_warn(rc);
  return rc;
}