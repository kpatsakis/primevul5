displen (s)
     const char *s;
{
#if defined (HANDLE_MULTIBYTE)
  wchar_t *wcstr;
  size_t wclen, slen;

  wcstr = 0;
  slen = mbstowcs (wcstr, s, 0);
  if (slen == -1)
    slen = 0;
  wcstr = (wchar_t *)xmalloc (sizeof (wchar_t) * (slen + 1));
  mbstowcs (wcstr, s, slen + 1);
  wclen = wcswidth (wcstr, slen);
  free (wcstr);
  return ((int)wclen);
#else
  return (STRLEN (s));
#endif
}