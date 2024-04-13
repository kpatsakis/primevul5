prepend_default_options (char const *options, int *pargc, char ***pargv)
{
  if (options && *options)
    {
      char *buf = xmalloc (strlen (options) + 1);
      size_t prepended = prepend_args (options, buf, NULL);
      int argc = *pargc;
      char *const *argv = *pargv;
      char **pp;
      enum { MAX_ARGS = MIN (INT_MAX, SIZE_MAX / sizeof *pp - 1) };
      if (MAX_ARGS - argc < prepended)
        xalloc_die ();
      pp = xmalloc ((prepended + argc + 1) * sizeof *pp);
      *pargc = prepended + argc;
      *pargv = pp;
      *pp++ = *argv++;
      pp += prepend_args (options, buf, pp);
      while ((*pp++ = *argv++))
        continue;
      return prepended;
    }

  return 0;
}