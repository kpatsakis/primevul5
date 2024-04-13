sv_histtimefmt (name)
     char *name;
{
  SHELL_VAR *v;

  v = find_variable (name);
  history_write_timestamps = (v != 0);
}