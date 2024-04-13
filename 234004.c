path_end (const char *url)
{
  enum url_scheme scheme = url_scheme (url);
  const char *seps;
  if (scheme == SCHEME_INVALID)
    scheme = SCHEME_HTTP;       /* use http semantics for rel links */
  /* +2 to ignore the first two separators ':' and '/' */
  seps = init_seps (scheme) + 2;
  return strpbrk_or_eos (url, seps);
}