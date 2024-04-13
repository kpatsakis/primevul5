void Regexp_processor_pcre::pcre_exec_warn(int rc) const
{
  char buf[64];
  const char *errmsg= NULL;
  THD *thd= current_thd;

  /*
    Make a descriptive message only for those pcre_exec() error codes
    that can actually happen in MariaDB.
  */
  switch (rc)
  {
  case PCRE_ERROR_NULL:
    errmsg= "pcre_exec: null argument passed";
    break;
  case PCRE_ERROR_BADOPTION:
    errmsg= "pcre_exec: bad option";
    break;
  case PCRE_ERROR_BADMAGIC:
    errmsg= "pcre_exec: bad magic - not a compiled regex";
    break;
  case PCRE_ERROR_UNKNOWN_OPCODE:
    errmsg= "pcre_exec: error in compiled regex";
    break;
  case PCRE_ERROR_NOMEMORY:
    errmsg= "pcre_exec: Out of memory";
    break;
  case PCRE_ERROR_NOSUBSTRING:
    errmsg= "pcre_exec: no substring";
    break;
  case PCRE_ERROR_MATCHLIMIT:
    errmsg= "pcre_exec: match limit exceeded";
    break;
  case PCRE_ERROR_CALLOUT:
    errmsg= "pcre_exec: callout error";
    break;
  case PCRE_ERROR_BADUTF8:
    errmsg= "pcre_exec: Invalid utf8 byte sequence in the subject string";
    break;
  case PCRE_ERROR_BADUTF8_OFFSET:
    errmsg= "pcre_exec: Started at invalid location within utf8 byte sequence";
    break;
  case PCRE_ERROR_PARTIAL:
    errmsg= "pcre_exec: partial match";
    break;
  case PCRE_ERROR_INTERNAL:
    errmsg= "pcre_exec: internal error";
    break;
  case PCRE_ERROR_BADCOUNT:
    errmsg= "pcre_exec: ovesize is negative";
    break;
  case PCRE_ERROR_RECURSIONLIMIT:
    my_snprintf(buf, sizeof(buf), "pcre_exec: recursion limit of %ld exceeded",
                m_pcre_extra.match_limit_recursion);
    errmsg= buf;
    break;
  case PCRE_ERROR_BADNEWLINE:
    errmsg= "pcre_exec: bad newline options";
    break;
  case PCRE_ERROR_BADOFFSET:
    errmsg= "pcre_exec: start offset negative or greater than string length";
    break;
  case PCRE_ERROR_SHORTUTF8:
    errmsg= "pcre_exec: ended in middle of utf8 sequence";
    break;
  case PCRE_ERROR_JIT_STACKLIMIT:
    errmsg= "pcre_exec: insufficient stack memory for JIT compile";
    break;
  case PCRE_ERROR_RECURSELOOP:
    errmsg= "pcre_exec: Recursion loop detected";
    break;
  case PCRE_ERROR_BADMODE:
    errmsg= "pcre_exec: compiled pattern passed to wrong bit library function";
    break;
  case PCRE_ERROR_BADENDIANNESS:
    errmsg= "pcre_exec: compiled pattern passed to wrong endianness processor";
    break;
  case PCRE_ERROR_JIT_BADOPTION:
    errmsg= "pcre_exec: bad jit option";
    break;
  case PCRE_ERROR_BADLENGTH:
    errmsg= "pcre_exec: negative length";
    break;
  default:
    /*
      As other error codes should normally not happen,
      we just report the error code without textual description
      of the code.
    */
    my_snprintf(buf, sizeof(buf), "pcre_exec: Internal error (%d)", rc);
    errmsg= buf;
  }
  push_warning_printf(thd, Sql_condition::WARN_LEVEL_WARN,
                      ER_REGEXP_ERROR, ER_THD(thd, ER_REGEXP_ERROR), errmsg);
}