coproc_wclose (cp, fd)
     struct coproc *cp;
     int fd;
{
  if (cp->c_wfd >= 0 && cp->c_wfd == fd)
    {
      close (cp->c_wfd);
      cp->c_wfd = -1;
    }
}