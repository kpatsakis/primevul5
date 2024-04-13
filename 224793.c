cpl_fdchk (fd)
     int fd;
{
  struct cpelement *cpe;

  for (cpe = coproc_list.head; cpe; cpe = cpe->next)
    coproc_checkfd (cpe->coproc, fd);
}