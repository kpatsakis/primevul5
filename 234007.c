url_escape_1 (const char *s, unsigned char mask, bool allow_passthrough)
{
  const char *p1;
  char *p2, *newstr;
  int newlen;
  int addition = 0;

  for (p1 = s; *p1; p1++)
    if (urlchr_test (*p1, mask))
      addition += 2;            /* Two more characters (hex digits) */

  if (!addition)
    return allow_passthrough ? (char *)s : xstrdup (s);

  newlen = (p1 - s) + addition;
  newstr = xmalloc (newlen + 1);

  p1 = s;
  p2 = newstr;
  while (*p1)
    {
      /* Quote the characters that match the test mask. */
      if (urlchr_test (*p1, mask))
        {
          unsigned char c = *p1++;
          *p2++ = '%';
          *p2++ = XNUM_TO_DIGIT (c >> 4);
          *p2++ = XNUM_TO_DIGIT (c & 0xf);
        }
      else
        *p2++ = *p1++;
    }
  assert (p2 - newstr == newlen);
  *p2 = '\0';

  return newstr;
}