url_string (const struct url *url, enum url_auth_mode auth_mode)
{
  int size;
  char *result, *p;
  char *quoted_host, *quoted_user = NULL, *quoted_passwd = NULL;

  int scheme_port = supported_schemes[url->scheme].default_port;
  const char *scheme_str = supported_schemes[url->scheme].leading_string;
  int fplen = full_path_length (url);

  bool brackets_around_host;

  assert (scheme_str != NULL);

  /* Make sure the user name and password are quoted. */
  if (url->user)
    {
      if (auth_mode != URL_AUTH_HIDE)
        {
          quoted_user = url_escape_allow_passthrough (url->user);
          if (url->passwd)
            {
              if (auth_mode == URL_AUTH_HIDE_PASSWD)
                quoted_passwd = (char *) HIDDEN_PASSWORD;
              else
                quoted_passwd = url_escape_allow_passthrough (url->passwd);
            }
        }
    }

  /* In the unlikely event that the host name contains non-printable
     characters, quote it for displaying to the user.  */
  quoted_host = url_escape_allow_passthrough (url->host);

  /* Undo the quoting of colons that URL escaping performs.  IPv6
     addresses may legally contain colons, and in that case must be
     placed in square brackets.  */
  if (quoted_host != url->host)
    unescape_single_char (quoted_host, ':');
  brackets_around_host = strchr (quoted_host, ':') != NULL;

  size = (strlen (scheme_str)
          + strlen (quoted_host)
          + (brackets_around_host ? 2 : 0)
          + fplen
          + 1);
  if (url->port != scheme_port)
    size += 1 + numdigit (url->port);
  if (quoted_user)
    {
      size += 1 + strlen (quoted_user);
      if (quoted_passwd)
        size += 1 + strlen (quoted_passwd);
    }

  p = result = xmalloc (size);

  APPEND (p, scheme_str);
  if (quoted_user)
    {
      APPEND (p, quoted_user);
      if (quoted_passwd)
        {
          *p++ = ':';
          APPEND (p, quoted_passwd);
        }
      *p++ = '@';
    }

  if (brackets_around_host)
    *p++ = '[';
  APPEND (p, quoted_host);
  if (brackets_around_host)
    *p++ = ']';
  if (url->port != scheme_port)
    {
      *p++ = ':';
      p = number_to_string (p, url->port);
    }

  full_path_write (url, p);
  p += fplen;
  *p++ = '\0';

  assert (p - result == size);

  if (quoted_user && quoted_user != url->user)
    xfree (quoted_user);
  if (quoted_passwd && auth_mode == URL_AUTH_SHOW
      && quoted_passwd != url->passwd)
    xfree (quoted_passwd);
  if (quoted_host != url->host)
    xfree (quoted_host);

  return result;
}