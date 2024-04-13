static inline void convert_dash_to_underscore(char *str, int len)
{
  for (char *p= str; p <= str+len; p++)
    if (*p == '-')
      *p= '_';
}