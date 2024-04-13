unescape_single_char (char *str, char chr)
{
  const char c1 = XNUM_TO_DIGIT (chr >> 4);
  const char c2 = XNUM_TO_DIGIT (chr & 0xf);
  char *h = str;                /* hare */
  char *t = str;                /* tortoise */
  for (; *h; h++, t++)
    {
      if (h[0] == '%' && h[1] == c1 && h[2] == c2)
        {
          *t = chr;
          h += 2;
        }
      else
        *t = *h;
    }
  *t = '\0';
}