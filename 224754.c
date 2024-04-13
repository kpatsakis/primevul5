bind_lastarg (arg)
     char *arg;
{
  SHELL_VAR *var;

  if (arg == 0)
    arg = "";
  var = bind_variable ("_", arg, 0);
  VUNSETATTR (var, att_exported);
}