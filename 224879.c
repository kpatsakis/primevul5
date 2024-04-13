cpl_search (pid)
     pid_t pid;
{
  struct cpelement *cpe;

  for (cpe = coproc_list.head ; cpe; cpe = cpe->next)
    if (cpe->coproc->c_pid == pid)
      return cpe;
  return (struct cpelement *)NULL;
}