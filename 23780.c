memchr_kwset (char const *s, size_t n, kwset_t kwset)
{
  if (kwset->gc1help < 0)
    return memchr (s, kwset->gc1, n);
  int small_heuristic = 2;
  int small = (- (uintptr_t) s % sizeof (long)
               + small_heuristic * sizeof (long));
  size_t ntrans = kwset->gc1help < NCHAR && small < n ? small : n;
  char const *slim = s + ntrans;
  for (; s < slim; s++)
    if (kwset->trans[U(*s)] == kwset->gc1)
      return s;
  n -= ntrans;
  return n == 0 ? NULL : memchr2 (s, kwset->gc1, kwset->gc1help, n);
}