variable_in_context (var)
     SHELL_VAR *var;
{
  return (invisible_p (var) == 0 && local_p (var) && var->context == variable_context);
}