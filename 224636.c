set_context (var)
     SHELL_VAR *var;
{
  return (var->context = variable_context);
}