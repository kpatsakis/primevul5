are_urls_equal (const char *u1, const char *u2)
{
  const char *p, *q;
  int pp, qq;
  char ch1, ch2;
  assert(u1 && u2);

  p = u1;
  q = u2;

  while (*p && *q
         && (pp = getchar_from_escaped_string (p, &ch1))
         && (qq = getchar_from_escaped_string (q, &ch2))
         && (c_tolower(ch1) == c_tolower(ch2)))
    {
      p += pp;
      q += qq;
    }

  return (*p == 0 && *q == 0 ? true : false);
}