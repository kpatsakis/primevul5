getcoprocbyname (name)
     const char *name;
{
#if MULTIPLE_COPROCS
  struct cpelement *p;

  p = cpl_searchbyname (name);
  return (p ? p->coproc : 0);
#else
  return ((sh_coproc.c_name && STREQ (sh_coproc.c_name, name)) ? &sh_coproc : 0);
#endif
}