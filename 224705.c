coproc_alloc (name, pid)
     char *name;
     pid_t pid;
{
  struct coproc *cp;

#if MULTIPLE_COPROCS
  cp = (struct coproc *)xmalloc (sizeof (struct coproc));
#else
  cp = &sh_coproc;
#endif
  coproc_init (cp);

  cp->c_name = savestring (name);
  cp->c_pid = pid;

#if MULTIPLE_COPROCS
  cpl_add (cp);
#endif

  return (cp);
}