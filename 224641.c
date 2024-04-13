coproc_reap ()
{
#if MULTIPLE_COPROCS
  cpl_reap ();
#else
  struct coproc *cp;

  cp = &sh_coproc;		/* XXX - will require changes for multiple coprocs */
  if (cp && (cp->c_flags & COPROC_DEAD))
    coproc_dispose (cp);
#endif
}