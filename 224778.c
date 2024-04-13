print_assignment (var)
     SHELL_VAR *var;
{
  if (var_isset (var) == 0)
    return;

  if (function_p (var))
    {
      printf ("%s", var->name);
      print_var_function (var);
      printf ("\n");
    }
#if defined (ARRAY_VARS)
  else if (array_p (var))
    print_array_assignment (var, 0);
  else if (assoc_p (var))
    print_assoc_assignment (var, 0);
#endif /* ARRAY_VARS */
  else
    {
      printf ("%s=", var->name);
      print_var_value (var, 1);
      printf ("\n");
    }
}