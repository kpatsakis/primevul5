static PCRE2_SPTR SLJIT_FUNC do_script_run_utf(PCRE2_SPTR ptr, PCRE2_SPTR endptr)
{
  if (PRIV(script_run)(ptr, endptr, TRUE))
    return endptr;
  return NULL;
}