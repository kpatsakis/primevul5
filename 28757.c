has_insecure_name_p (const char *s)
{
  if (*s == '/')
    return true;

  if (strstr (s, "../") != 0)
    return true;

  return false;
}
