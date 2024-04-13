cpl_searchbyname (name)
     const char *name;
{
  struct cpelement *cp;

  for (cp = coproc_list.head ; cp; cp = cp->next)
    if (STREQ (cp->coproc->c_name, name))
      return cp;
  return (struct cpelement *)NULL;
}