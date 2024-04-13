buf_all_zeros (char *buf, int bufsize)
{
  int	i;
  for (i = 0; i < bufsize; ++i)
    {
      if (*buf++ != '\0')
	return 0;
    }
  return 1;
}