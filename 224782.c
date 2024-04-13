ansic_quote (str, flags, rlen)
     char *str;
     int flags, *rlen;
{
  char *r, *ret, *s;
  int l, rsize, sindex;
  unsigned char c;
  size_t slen, clen;
#if defined (HANDLE_MULTIBYTE)
  int b;
  wchar_t wc;
#endif

  if (str == 0 || *str == 0)
    return ((char *)0);

  l = strlen (str);
  rsize = 4 * l + 4;
  r = ret = (char *)xmalloc (rsize);

  *r++ = '$';
  *r++ = '\'';

  s = str;
  slen = strlen (str);

  for (s = str; c = *s; s++)
    {
      l = 1;		/* 1 == add backslash; 0 == no backslash */
      clen = 1;

      switch (c)
	{
	case ESC: c = 'E'; break;
#ifdef __STDC__
	case '\a': c = 'a'; break;
	case '\v': c = 'v'; break;
#else
	case 0x07: c = 'a'; break;
	case 0x0b: c = 'v'; break;
#endif

	case '\b': c = 'b'; break;
	case '\f': c = 'f'; break;
	case '\n': c = 'n'; break;
	case '\r': c = 'r'; break;
	case '\t': c = 't'; break;
	case '\\':
	case '\'':
	  break;
	default:
#if defined (HANDLE_MULTIBYTE)
	  b = is_basic (c);
	  if ((b == 0 && ((clen = mbrtowc (&wc, s, MB_CUR_MAX, 0)) < 0 || iswprint (wc) == 0)) ||
	      (b == 1 && ISPRINT (c) == 0))
#else
	  if (ISPRINT (c) == 0)
#endif
	    {
	      *r++ = '\\';
	      *r++ = TOCHAR ((c >> 6) & 07);
	      *r++ = TOCHAR ((c >> 3) & 07);
	      *r++ = TOCHAR (c & 07);
	      continue;
	    }
	  l = 0;
	  break;
	}
      if (l)
	*r++ = '\\';

      if (clen == 1)
	*r++ = c;
      else
	for (b = 0; b < (int)clen; c = b ? *++s : c)
	  *r++ = c;
    }

  *r++ = '\'';
  *r = '\0';
  if (rlen)
    *rlen = r - ret;
  return ret;
}