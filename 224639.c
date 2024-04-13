coproc_fdclose (cp, fd)
     struct coproc *cp;
     int fd;
{
  coproc_rclose (cp, fd);
  coproc_wclose (cp, fd);
  coproc_setvars (cp);
}