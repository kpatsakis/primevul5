ansic_shouldquote (string)
     const char *string;
{
  const char *s;
  unsigned char c;

  if (string == 0)
    return 0;

  for (s = string; c = *s; s++)
    {
#if defined (HANDLE_MULTIBYTE)
      if (is_basic (c) == 0)
	return (ansic_wshouldquote (s));
#endif
      if (ISPRINT (c) == 0)
	return 1;
    }

  return 0;
}