url_scheme (const char *url)
{
  int i;

  for (i = 0; supported_schemes[i].leading_string; i++)
    if (0 == strncasecmp (url, supported_schemes[i].leading_string,
                          strlen (supported_schemes[i].leading_string)))
      {
        if (!(supported_schemes[i].flags & scm_disabled))
          return (enum url_scheme) i;
        else
          return SCHEME_INVALID;
      }

  return SCHEME_INVALID;
}