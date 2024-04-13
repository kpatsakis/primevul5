init_seps (enum url_scheme scheme)
{
  static char seps[8] = ":/";
  char *p = seps + 2;
  int flags = supported_schemes[scheme].flags;

  if (flags & scm_has_params)
    *p++ = ';';
  if (flags & scm_has_query)
    *p++ = '?';
  if (flags & scm_has_fragment)
    *p++ = '#';
  *p = '\0';
  return seps;
}