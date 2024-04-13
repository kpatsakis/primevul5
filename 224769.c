set_var_read_only (name)
     char *name;
{
  SHELL_VAR *entry;

  FIND_OR_MAKE_VARIABLE (name, entry);
  VSETATTR (entry, att_readonly);
}