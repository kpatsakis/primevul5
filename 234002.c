url_error (const char *url, int error_code)
{
  assert (error_code >= 0 && ((size_t) error_code) < countof (parse_errors));

  if (error_code == PE_UNSUPPORTED_SCHEME)
    {
      char *error, *p;
      char *scheme = xstrdup (url);
      assert (url_has_scheme (url));

      if ((p = strchr (scheme, ':')))
        *p = '\0';
      if (!c_strcasecmp (scheme, "https"))
        error = aprintf (_("HTTPS support not compiled in"));
      else
        error = aprintf (_(parse_errors[error_code]), quote (scheme));
      xfree (scheme);

      return error;
    }
  else
    return xstrdup (_(parse_errors[error_code]));
}