static inline void convert_underscore_to_dash(char *str, int len)
{
  for (char *p= str; p <= str+len; p++)
    if (*p == '_')
      *p= '-';
}