static inline const char *strchrnul(const char *str, char x)
{
  const char *p = strchr(str, x);
  return p ? p : str + strlen(str);
}