url_escape (const char *s)
{
  return url_escape_1 (s, urlchr_unsafe, false);
}