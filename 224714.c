get_bash_command (var)
     SHELL_VAR *var;
{
  char *p;

  if (the_printed_command_except_trap)
    p = savestring (the_printed_command_except_trap);
  else
    {
      p = (char *)xmalloc (1);
      p[0] = '\0';
    }
  FREE (value_cell (var));
  var_setvalue (var, p);
  return (var);
}