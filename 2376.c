static PCRE2_SPTR SLJIT_FUNC do_script_run(PCRE2_SPTR ptr, PCRE2_SPTR endptr)
{
  if (PRIV(script_run)(ptr, endptr, FALSE))
    return endptr;
  return NULL;
}