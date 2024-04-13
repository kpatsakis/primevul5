coproc_dispose (cp)
     struct coproc *cp;
{
  if (cp == 0)
    return;

  coproc_unsetvars (cp);
  FREE (cp->c_name);
  coproc_close (cp);
#if MULTIPLE_COPROCS
  coproc_free (cp);
#else
  coproc_init (cp);
#endif
}