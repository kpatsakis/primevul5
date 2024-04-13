static bool filename_char(int const c)
{
  return isalnum(c) || (c == '-') || (c == '_') || (c == '.');
}