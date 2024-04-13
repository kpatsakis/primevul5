create_variable_tables ()
{
  if (shell_variables == 0)
    {
      shell_variables = global_variables = new_var_context ((char *)NULL, 0);
      shell_variables->scope = 0;
      shell_variables->table = hash_create (0);
    }

  if (shell_functions == 0)
    shell_functions = hash_create (0);

#if defined (DEBUGGER)
  if (shell_function_defs == 0)
    shell_function_defs = hash_create (0);
#endif
}