my_unescape(CHARSET_INFO *cs, char *to, const char *str, const char *end,
            int sep, bool backslash_escapes)
{
  char *start= to;
  for ( ; str != end ; str++)
  {
#ifdef USE_MB
    int l;
    if (use_mb(cs) && (l= my_ismbchar(cs, str, end)))
    {
      while (l--)
        *to++ = *str++;
      str--;
      continue;
    }
#endif
    if (backslash_escapes && *str == '\\' && str + 1 != end)
    {
      switch(*++str) {
      case 'n':
        *to++='\n';
        break;
      case 't':
        *to++= '\t';
        break;
      case 'r':
        *to++ = '\r';
        break;
      case 'b':
        *to++ = '\b';
        break;
      case '0':
        *to++= 0;                      // Ascii null
        break;
      case 'Z':                        // ^Z must be escaped on Win32
        *to++='\032';
        break;
      case '_':
      case '%':
        *to++= '\\';                   // remember prefix for wildcard
        /* Fall through */
      default:
        *to++= *str;
        break;
      }
    }
    else if (*str == sep)
      *to++= *str++;                // Two ' or "
    else
      *to++ = *str;
  }
  *to= 0;
  return to - start;
}