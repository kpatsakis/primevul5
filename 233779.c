nlscan (char const *lim)
{
  size_t newlines = 0;
  char const *beg;
  for (beg = lastnl; beg < lim; beg++)
    {
      beg = memchr (beg, eolbyte, lim - beg);
      if (!beg)
        break;
      newlines++;
    }
  totalnl = add_count (totalnl, newlines);
  lastnl = lim;
}