coproc_pidchk (pid, status)
     pid_t pid;
{
  struct coproc *cp;

#if MULTIPLE_COPROCS
  struct cpelement *cpe;

  cpe = cpl_delete (pid);
  cp = cpe ? cpe->coproc : 0;
#else
  cp = getcoprocbypid (pid);
#endif
  if (cp)
    {
#if 0
      itrace("coproc_pidchk: pid %d has died", pid);
#endif
      cp->c_status = status;
      cp->c_flags |= COPROC_DEAD;
      cp->c_flags &= ~COPROC_RUNNING;
#if MULTIPLE_COPROCS
      coproc_dispose (cp);
#else
      coproc_unsetvars (cp);
#endif
    }
}