url_escape_unsafe_and_reserved (const char *s)
{
  return url_escape_1 (s, urlchr_unsafe|urlchr_reserved, false);
}