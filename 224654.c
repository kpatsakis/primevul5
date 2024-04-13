coproc_init (cp)
     struct coproc *cp;
{
  cp->c_name = 0;
  cp->c_pid = NO_PID;
  cp->c_rfd = cp->c_wfd = -1;
  cp->c_rsave = cp->c_wsave = -1;
  cp->c_flags = cp->c_status = 0;  
}