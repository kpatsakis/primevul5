getcoprocbypid (pid)
     pid_t pid;
{
#if MULTIPLE_COPROCS
  struct cpelement *p;

  p = cpl_search (pid);
  return (p ? p->coproc : 0);
#else
  return (pid == sh_coproc.c_pid ? &sh_coproc : 0);
#endif
}