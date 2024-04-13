static bool address_char(int const c)
{
  return filename_char(c) ||
         (c == ':') || (c == '[') || (c == ']') || (c == '/');
}