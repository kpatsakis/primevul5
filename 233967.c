strpbrk_or_eos (const char *s, const char *accept)
{
  char *p = strpbrk (s, accept);
  if (!p)
    p = strchr (s, '\0');
  return p;
}