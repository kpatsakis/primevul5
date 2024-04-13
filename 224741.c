sv_xtracefd (name)
     char *name;
{
  SHELL_VAR *v;
  char *t, *e;
  int fd;
  FILE *fp;

  v = find_variable (name);
  if (v == 0)
    {
      xtrace_reset ();
      return;
    }

  t = value_cell (v);
  if (t == 0 || *t == 0)
    xtrace_reset ();
  else
    {
      fd = (int)strtol (t, &e, 10);
      if (e != t && *e == '\0' && sh_validfd (fd))
	{
	  fp = fdopen (fd, "w");
	  if (fp == 0)
	    internal_error (_("%s: %s: cannot open as FILE"), name, value_cell (v));
	  else
	    xtrace_set (fd, fp);
	}
      else
	internal_error (_("%s: %s: invalid value for trace file descriptor"), name, value_cell (v));
    }
}