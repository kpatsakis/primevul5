coproc_flush ()
{
#if MULTIPLE_COPROCS
  cpl_flush ();
#else
  coproc_dispose (&sh_coproc);
#endif
}