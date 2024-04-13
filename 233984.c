char_needs_escaping (const char *p)
{
  if (*p == '%')
    {
      if (c_isxdigit (*(p + 1)) && c_isxdigit (*(p + 2)))
        return false;
      else
        /* Garbled %.. sequence: encode `%'. */
        return true;
    }
  else if (URL_UNSAFE_CHAR (*p) && !URL_RESERVED_CHAR (*p))
    return true;
  else
    return false;
}