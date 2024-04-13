cpl_closeall ()
{
  struct cpelement *cpe;

  for (cpe = coproc_list.head; cpe; cpe = cpe->next)
    coproc_close (cpe->coproc);
}