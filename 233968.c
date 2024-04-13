schemes_are_similar_p (enum url_scheme a, enum url_scheme b)
{
  if (a == b)
    return true;
#ifdef HAVE_SSL
  if ((a == SCHEME_HTTP && b == SCHEME_HTTPS)
      || (a == SCHEME_HTTPS && b == SCHEME_HTTP))
    return true;
#endif
  return false;
}