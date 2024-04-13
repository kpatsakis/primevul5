coproc_fdsave (cp)
     struct coproc *cp;
{
  cp->c_rsave = cp->c_rfd;
  cp->c_wsave = cp->c_wfd;
}