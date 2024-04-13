url_unescape_except_reserved (char *s)
{
  url_unescape_1 (s, urlchr_reserved);
}