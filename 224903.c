cpl_prune ()
{
  struct cpelement *cp;

  while (coproc_list.head && coproc_list.ncoproc > COPROC_MAX)
    {
      cp = coproc_list.head;
      coproc_list.head = coproc_list.head->next;
      coproc_dispose (cp->coproc);
      cpe_dispose (cp);
      coproc_list.ncoproc--;
    }
}