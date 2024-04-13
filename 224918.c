coproc_fdrestore (cp)
     struct coproc *cp;
{
  cp->c_rfd = cp->c_rsave;
  cp->c_wfd = cp->c_wsave;
}