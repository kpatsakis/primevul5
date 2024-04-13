cpl_add (cp)
     Coproc *cp;
{
  struct cpelement *cpe;

  cpe = cpe_alloc (cp);

  if (coproc_list.head == 0)
    {
      coproc_list.head = coproc_list.tail = cpe;
      coproc_list.ncoproc = 0;			/* just to make sure */
    }
  else
    {
      coproc_list.tail->next = cpe;
      coproc_list.tail = cpe;
    }
  coproc_list.ncoproc++;

  return cpe;
}