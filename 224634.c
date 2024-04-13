cpl_flush ()
{
  struct cpelement *cpe, *p;

  for (cpe = coproc_list.head; cpe; )
    {
      p = cpe;
      cpe = cpe->next;

      coproc_dispose (p->coproc);
      cpe_dispose (p);
    }

  coproc_list.head = coproc_list.tail = 0;
  coproc_list.ncoproc = 0;
}