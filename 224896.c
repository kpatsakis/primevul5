print_var_function (var)
     SHELL_VAR *var;
{
  char *x;

  if (function_p (var) && var_isset (var))
    {
      x = named_function_string ((char *)NULL, function_cell(var), FUNC_MULTILINE|FUNC_EXTERNAL);
      printf ("%s", x);
    }
}