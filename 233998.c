getchar_from_escaped_string (const char *str, char *c)
{
  const char *p = str;

  assert (str && *str);
  assert (c);

  if (p[0] == '%')
    {
      if (!c_isxdigit(p[1]) || !c_isxdigit(p[2]))
        {
          *c = '%';
          return 1;
        }
      else
        {
          if (p[2] == 0)
            return 0; /* error: invalid string */

          *c = X2DIGITS_TO_NUM (p[1], p[2]);
          if (URL_RESERVED_CHAR(*c))
            {
              *c = '%';
              return 1;
            }
          else
            return 3;
        }
    }
  else
    {
      *c = p[0];
    }

  return 1;
}