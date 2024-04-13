add_or_supercede_exported_var (assign, do_alloc)
     char *assign;
     int do_alloc;
{
  register int i;
  int equal_offset;

  equal_offset = assignment (assign, 0);
  if (equal_offset == 0)
    return (export_env);

  /* If this is a function, then only supersede the function definition.
     We do this by including the `=() {' in the comparison, like
     initialize_shell_variables does. */
  if (assign[equal_offset + 1] == '(' &&
     strncmp (assign + equal_offset + 2, ") {", 3) == 0)		/* } */
    equal_offset += 4;

  for (i = 0; i < export_env_index; i++)
    {
      if (STREQN (assign, export_env[i], equal_offset + 1))
	{
	  free (export_env[i]);
	  export_env[i] = do_alloc ? savestring (assign) : assign;
	  return (export_env);
	}
    }
  add_to_export_env (assign, do_alloc);
  return (export_env);
}