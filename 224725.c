chkexport (name)
     char *name;
{
  SHELL_VAR *v;

  v = find_variable (name);
  if (v && exported_p (v))
    {
      array_needs_making = 1;
      maybe_make_export_env ();
      return 1;
    }
  return 0;
}