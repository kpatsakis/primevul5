set_func_read_only (name)
     const char *name;
{
  SHELL_VAR *entry;

  entry = find_function (name);
  if (entry)
    VSETATTR (entry, att_readonly);
}