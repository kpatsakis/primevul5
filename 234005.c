convert_fname (const char *fname)
{
  char *converted_fname = (char *)fname;
#if HAVE_ICONV
  const char *from_encoding = opt.encoding_remote;
  const char *to_encoding = opt.locale;
  iconv_t cd;
  size_t len, done, inlen, outlen;
  char *s;
  const char *orig_fname = fname;;

  /* Defaults for remote and local encodings.  */
  if (!from_encoding)
    from_encoding = "UTF-8";
  if (!to_encoding)
    to_encoding = nl_langinfo (CODESET);

  cd = iconv_open (to_encoding, from_encoding);
  if (cd == (iconv_t)(-1))
    logprintf (LOG_VERBOSE, _("Conversion from %s to %s isn't supported\n"),
	       quote (from_encoding), quote (to_encoding));
  else
    {
      inlen = strlen (fname);
      len = outlen = inlen * 2;
      converted_fname = s = xmalloc (outlen + 1);
      done = 0;

      for (;;)
	{
	  if (iconv (cd, &fname, &inlen, &s, &outlen) != (size_t)(-1)
	      && iconv (cd, NULL, NULL, &s, &outlen) != (size_t)(-1))
	    {
	      *(converted_fname + len - outlen - done) = '\0';
	      iconv_close(cd);
	      DEBUGP (("Converted file name '%s' (%s) -> '%s' (%s)\n",
		       orig_fname, from_encoding, converted_fname, to_encoding));
	      xfree (orig_fname);
	      return converted_fname;
	    }

	  /* Incomplete or invalid multibyte sequence */
	  if (errno == EINVAL || errno == EILSEQ)
	    {
	      logprintf (LOG_VERBOSE,
			 _("Incomplete or invalid multibyte sequence encountered\n"));
	      xfree (converted_fname);
	      converted_fname = (char *)orig_fname;
	      break;
	    }
	  else if (errno == E2BIG) /* Output buffer full */
	    {
	      done = len;
	      len = outlen = done + inlen * 2;
	      converted_fname = xrealloc (converted_fname, outlen + 1);
	      s = converted_fname + done;
	    }
	  else /* Weird, we got an unspecified error */
	    {
	      logprintf (LOG_VERBOSE, _("Unhandled errno %d\n"), errno);
	      xfree (converted_fname);
	      converted_fname = (char *)orig_fname;
	      break;
	    }
	}
      DEBUGP (("Failed to convert file name '%s' (%s) -> '?' (%s)\n",
	       orig_fname, from_encoding, to_encoding));
    }

    iconv_close(cd);
#endif

  return converted_fname;
}