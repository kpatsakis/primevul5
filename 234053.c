from_ascii (char const *where, size_t digs, unsigned logbase)
{
  uintmax_t value = 0;
  char const *buf = where;
  char const *end = buf + digs;
  int overflow = 0;
  static char codetab[] = "0123456789ABCDEF";

  for (; *buf == ' '; buf++)
    {
      if (buf == end)
	return 0;
    }

  if (buf == end || *buf == 0)
    return 0;
  while (1)
    {
      unsigned d;
      
      char *p = strchr (codetab, toupper (*buf));
      if (!p)
	{
	  error (0, 0, _("Malformed number %.*s"), (int) digs, where);
	  break;
	}
      
      d = p - codetab;
      if ((d >> logbase) > 1)
	{
	  error (0, 0, _("Malformed number %.*s"), (int) digs, where);
	  break;
	}
      value += d;
      if (++buf == end || *buf == 0)
	break;
      overflow |= value ^ (value << logbase >> logbase);
      value <<= logbase;
    }
  if (overflow)
    error (0, 0, _("Archive value %.*s is out of range"),
	   (int) digs, where);
  return value;
}