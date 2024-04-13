sv_ifs (name)
     char *name;
{
  SHELL_VAR *v;

  v = find_variable ("IFS");
  setifs (v);
}