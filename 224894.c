get_bashpid (var)
     SHELL_VAR *var;
{
  int pid;
  char *p;

  pid = getpid ();
  p = itos (pid);

  FREE (value_cell (var));
  VSETATTR (var, att_integer|att_readonly);
  var_setvalue (var, p);
  return (var);
}