sh_mkdoublequoted (s, slen, flags)
     const char *s;
     int slen, flags;
{
  char *r, *ret;
  int rlen;

  rlen = (flags == 0) ? slen + 3 : (2 * slen) + 1;
  ret = r = (char *)xmalloc (rlen);
  
  *r++ = '"';
  while (*s)
    {
      if (flags && *s == '"')
	*r++ = '\\';
      *r++ = *s++;
    }
  *r++ = '"';
  *r = '\0';

  return ret;
}