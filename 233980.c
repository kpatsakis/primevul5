url_unescape_1 (char *s, unsigned char mask)
{
  char *t = s;                  /* t - tortoise */
  char *h = s;                  /* h - hare     */

  for (; *h; h++, t++)
    {
      if (*h != '%')
        {
        copychar:
          *t = *h;
        }
      else
        {
          char c;
          /* Do nothing if '%' is not followed by two hex digits. */
          if (!h[1] || !h[2] || !(c_isxdigit (h[1]) && c_isxdigit (h[2])))
            goto copychar;
          c = X2DIGITS_TO_NUM (h[1], h[2]);
          if (urlchr_test(c, mask))
            goto copychar;
          /* Don't unescape %00 because there is no way to insert it
             into a C string without effectively truncating it. */
          if (c == '\0')
            goto copychar;
          *t = c;
          h += 2;
        }
    }
  *t = '\0';
}