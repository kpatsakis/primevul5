reencode_escapes (const char *s)
{
  const char *p1;
  char *newstr, *p2;
  int oldlen, newlen;

  int encode_count = 0;

  /* First pass: inspect the string to see if there's anything to do,
     and to calculate the new length.  */
  for (p1 = s; *p1; p1++)
    if (char_needs_escaping (p1))
      ++encode_count;

  if (!encode_count)
    /* The string is good as it is. */
    return (char *) s;          /* C const model sucks. */

  oldlen = p1 - s;
  /* Each encoding adds two characters (hex digits).  */
  newlen = oldlen + 2 * encode_count;
  newstr = xmalloc (newlen + 1);

  /* Second pass: copy the string to the destination address, encoding
     chars when needed.  */
  p1 = s;
  p2 = newstr;

  while (*p1)
    if (char_needs_escaping (p1))
      {
        unsigned char c = *p1++;
        *p2++ = '%';
        *p2++ = XNUM_TO_DIGIT (c >> 4);
        *p2++ = XNUM_TO_DIGIT (c & 0xf);
      }
    else
      *p2++ = *p1++;

  *p2 = '\0';
  assert (p2 - newstr == newlen);
  return newstr;
}