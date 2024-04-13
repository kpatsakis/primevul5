coproc_fdchk (fd)
     int fd;
{
#if MULTIPLE_COPROCS
  cpl_fdchk (fd);
#else
  coproc_checkfd (&sh_coproc, fd);
#endif
}