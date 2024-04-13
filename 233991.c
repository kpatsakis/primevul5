url_escape_allow_passthrough (const char *s)
{
  return url_escape_1 (s, urlchr_unsafe, true);
}