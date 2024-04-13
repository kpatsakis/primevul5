mk_env_string (name, value)
     const char *name, *value;
{
  int name_len, value_len;
  char	*p;

  name_len = strlen (name);
  value_len = STRLEN (value);
  p = (char *)xmalloc (2 + name_len + value_len);
  strcpy (p, name);
  p[name_len] = '=';
  if (value && *value)
    strcpy (p + name_len + 1, value);
  else
    p[name_len + 1] = '\0';
  return (p);
}