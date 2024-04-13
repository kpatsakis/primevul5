append_uri_pathel (const char *b, const char *e, bool escaped,
                   struct growable *dest)
{
  const char *p;
  int quoted, outlen;

  int mask;
  if (opt.restrict_files_os == restrict_unix)
    mask = filechr_not_unix;
  else if (opt.restrict_files_os == restrict_vms)
    mask = filechr_not_vms;
  else
    mask = filechr_not_windows;
  if (opt.restrict_files_ctrl)
    mask |= filechr_control;

  /* Copy [b, e) to PATHEL and URL-unescape it. */
  if (escaped)
    {
      char *unescaped;
      BOUNDED_TO_ALLOCA (b, e, unescaped);
      url_unescape (unescaped);
      b = unescaped;
      e = unescaped + strlen (unescaped);
    }

  /* Defang ".." when found as component of path.  Remember that path
     comes from the URL and might contain malicious input.  */
  if (e - b == 2 && b[0] == '.' && b[1] == '.')
    {
      b = "%2E%2E";
      e = b + 6;
    }

  /* Walk the PATHEL string and check how many characters we'll need
     to quote.  */
  quoted = 0;
  for (p = b; p < e; p++)
    if (FILE_CHAR_TEST (*p, mask))
      ++quoted;

  /* Calculate the length of the output string.  e-b is the input
     string length.  Each quoted char introduces two additional
     characters in the string, hence 2*quoted.  */
  outlen = (e - b) + (2 * quoted);
  GROW (dest, outlen);

  if (!quoted)
    {
      /* If there's nothing to quote, we can simply append the string
         without processing it again.  */
      memcpy (TAIL (dest), b, outlen);
    }
  else
    {
      char *q = TAIL (dest);
      for (p = b; p < e; p++)
        {
          if (!FILE_CHAR_TEST (*p, mask))
            *q++ = *p;
          else
            {
              unsigned char ch = *p;
              *q++ = '%';
              *q++ = XNUM_TO_DIGIT (ch >> 4);
              *q++ = XNUM_TO_DIGIT (ch & 0xf);
            }
        }
      assert (q - TAIL (dest) == outlen);
    }

  /* Perform inline case transformation if required.  */
  if (opt.restrict_files_case == restrict_lowercase
      || opt.restrict_files_case == restrict_uppercase)
    {
      char *q;
      for (q = TAIL (dest); q < TAIL (dest) + outlen; ++q)
        {
          if (opt.restrict_files_case == restrict_lowercase)
            *q = c_tolower (*q);
          else
            *q = c_toupper (*q);
        }
    }

  TAIL_INCR (dest, outlen);
  append_null (dest);
}