stub_charset ()
{
  char *locale, *s, *t;

  locale = get_locale_var ("LC_CTYPE");
  if (locale == 0 || *locale == 0)
    {
      strcpy (charsetbuf, "ASCII");
      return charsetbuf;
    }
  s = strrchr (locale, '.');
  if (s)
    {
      strcpy (charsetbuf, s+1);
      t = strchr (charsetbuf, '@');
      if (t)
	*t = 0;
      return charsetbuf;
    }
  strcpy (charsetbuf, locale);
  return charsetbuf;
}