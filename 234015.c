parse_credentials (const char *beg, const char *end, char **user, char **passwd)
{
  char *colon;
  const char *userend;

  if (beg == end)
    return false;               /* empty user name */

  colon = memchr (beg, ':', end - beg);
  if (colon == beg)
    return false;               /* again empty user name */

  if (colon)
    {
      *passwd = strdupdelim (colon + 1, end);
      userend = colon;
      url_unescape (*passwd);
    }
  else
    {
      *passwd = NULL;
      userend = end;
    }
  *user = strdupdelim (beg, userend);
  url_unescape (*user);
  return true;
}