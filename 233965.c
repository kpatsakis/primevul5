url_valid_scheme (const char *url)
{
  enum url_scheme scheme = url_scheme (url);
  return scheme != SCHEME_INVALID;
}