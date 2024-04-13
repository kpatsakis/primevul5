coproc_checkfd (cp, fd)
     struct coproc *cp;
     int fd;
{
  int update;

  update = 0;
  if (cp->c_rfd >= 0 && cp->c_rfd == fd)
    update = cp->c_rfd = -1;
  if (cp->c_wfd >= 0 && cp->c_wfd == fd)
    update = cp->c_wfd = -1;
  if (update)
    coproc_setvars (cp);
}