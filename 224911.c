coproc_closeall ()
{
#if MULTIPLE_COPROCS
  cpl_closeall ();
#else
  coproc_close (&sh_coproc);	/* XXX - will require changes for multiple coprocs */
#endif
}