ansic_wshouldquote (string)
     const char *string;
{
  const wchar_t *wcs;
  wchar_t wcc;

  wchar_t *wcstr = NULL;
  size_t wclen, slen;


  slen = mbstowcs (wcstr, string, 0);

  if (slen == -1)
    slen = 0;
  wcstr = (wchar_t *)xmalloc (sizeof (wchar_t) * (slen + 1));
  mbstowcs (wcstr, string, slen + 1);

  for (wcs = wcstr; wcc = *wcs; wcs++)
    if (iswprint(wcc) == 0)
      {
	free (wcstr);
	return 1;
      }

  free (wcstr);
  return 0;
}