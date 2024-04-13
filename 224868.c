cpl_delete (pid)
     pid_t pid;
{
  struct cpelement *prev, *p;

  for (prev = p = coproc_list.head; p; prev = p, p = p->next)
    if (p->coproc->c_pid == pid)
      {
        prev->next = p->next;	/* remove from list */
        break;
      }

  if (p == 0)
    return 0;		/* not found */

#if defined (DEBUG)
  itrace("cpl_delete: deleting %d", pid);
#endif

  /* Housekeeping in the border cases. */
  if (p == coproc_list.head)
    coproc_list.head = coproc_list.head->next;
  else if (p == coproc_list.tail)
    coproc_list.tail = prev;

  coproc_list.ncoproc--;
  if (coproc_list.ncoproc == 0)
    coproc_list.head = coproc_list.tail = 0;
  else if (coproc_list.ncoproc == 1)
    coproc_list.tail = coproc_list.head;		/* just to make sure */

  return (p);
}