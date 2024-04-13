coproc_close (cp)
     struct coproc *cp;
{
  if (cp->c_rfd >= 0)
    {
      close (cp->c_rfd);
      cp->c_rfd = -1;
    }
  if (cp->c_wfd >= 0)
    {
      close (cp->c_wfd);
      cp->c_wfd = -1;
    }
  cp->c_rsave = cp->c_wsave = -1;
}