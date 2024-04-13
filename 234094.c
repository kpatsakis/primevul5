to_ascii (char *where, uintmax_t v, size_t digits, unsigned logbase, bool nul)
{
  static char codetab[] = "0123456789ABCDEF";

  if (nul)
    where[--digits] = 0;
  while (digits > 0)
    {
      where[--digits] = codetab[(v & ((1 << logbase) - 1))];
      v >>= logbase;
    }

  return v != 0;
}