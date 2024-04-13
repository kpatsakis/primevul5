coproc_rclose (cp, fd)
     struct coproc *cp;
     int fd;
{
  if (cp->c_rfd >= 0 && cp->c_rfd == fd)
    {
      close (cp->c_rfd);
      cp->c_rfd = -1;
    }
}