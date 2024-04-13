lowercase_str (char *str)
{
  bool changed = false;
  for (; *str; str++)
    if (c_isupper (*str))
      {
        changed = true;
        *str = c_tolower (*str);
      }
  return changed;
}