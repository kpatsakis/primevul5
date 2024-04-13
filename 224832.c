cpe_alloc (cp)
     Coproc *cp;
{
  struct cpelement *cpe;

  cpe = (struct cpelement *)xmalloc (sizeof (struct cpelement));
  cpe->coproc = cp;
  cpe->next = (struct cpelement *)0;
  return cpe;
}