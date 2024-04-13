rewrite_shorthand_url (const char *url)
{
  const char *p;
  char *ret;

  if (url_scheme (url) != SCHEME_INVALID)
    return NULL;

  /* Look for a ':' or '/'.  The former signifies NcFTP syntax, the
     latter Netscape.  */
  p = strpbrk (url, ":/");
  if (p == url)
    return NULL;

  /* If we're looking at "://", it means the URL uses a scheme we
     don't support, which may include "https" when compiled without
     SSL support.  Don't bogusly rewrite such URLs.  */
  if (p && p[0] == ':' && p[1] == '/' && p[2] == '/')
    return NULL;

  if (p && *p == ':')
    {
      /* Colon indicates ftp, as in foo.bar.com:path.  Check for
         special case of http port number ("localhost:10000").  */
      int digits = strspn (p + 1, "0123456789");
      if (digits && (p[1 + digits] == '/' || p[1 + digits] == '\0'))
        goto http;

      /* Turn "foo.bar.com:path" to "ftp://foo.bar.com/path". */
      if ((ret = aprintf ("ftp://%s", url)) != NULL)
        ret[6 + (p - url)] = '/';
    }
  else
    {
    http:
      /* Just prepend "http://" to URL. */
      ret = aprintf ("http://%s", url);
    }
  return ret;
}