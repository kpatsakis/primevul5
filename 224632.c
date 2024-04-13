sh_double_quote (string)
     const char *string;
{
  register unsigned char c;
  char *result, *r;
  const char *s;

  result = (char *)xmalloc (3 + (2 * strlen (string)));
  r = result;
  *r++ = '"';

  for (s = string; s && (c = *s); s++)
    {
      /* Backslash-newline disappears within double quotes, so don't add one. */
      if ((sh_syntaxtab[c] & CBSDQUOTE) && c != '\n')
	*r++ = '\\';
      else if (c == CTLESC || c == CTLNUL)
	*r++ = CTLESC;		/* could be '\\'? */

      *r++ = c;
    }

  *r++ = '"';
  *r = '\0';

  return (result);
}