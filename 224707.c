maybe_make_export_env ()
{
  register char **temp_array;
  int new_size;
  VAR_CONTEXT *tcxt;

  if (array_needs_making)
    {
      if (export_env)
	strvec_flush (export_env);

      /* Make a guess based on how many shell variables and functions we
	 have.  Since there will always be array variables, and array
	 variables are not (yet) exported, this will always be big enough
	 for the exported variables and functions. */
      new_size = n_shell_variables () + HASH_ENTRIES (shell_functions) + 1 +
		 HASH_ENTRIES (temporary_env);
      if (new_size > export_env_size)
	{
	  export_env_size = new_size;
	  export_env = strvec_resize (export_env, export_env_size);
	  environ = export_env;
	}
      export_env[export_env_index = 0] = (char *)NULL;

      /* Make a dummy variable context from the temporary_env, stick it on
	 the front of shell_variables, call make_var_export_array on the
	 whole thing to flatten it, and convert the list of SHELL_VAR *s
	 to the form needed by the environment. */
      if (temporary_env)
	{
	  tcxt = new_var_context ((char *)NULL, 0);
	  tcxt->table = temporary_env;
	  tcxt->down = shell_variables;
	}
      else
	tcxt = shell_variables;
      
      temp_array = make_var_export_array (tcxt);
      if (temp_array)
	add_temp_array_to_env (temp_array, 0, 0);

      if (tcxt != shell_variables)
	free (tcxt);

#if defined (RESTRICTED_SHELL)
      /* Restricted shells may not export shell functions. */
      temp_array = restricted ? (char **)0 : make_func_export_array ();
#else
      temp_array = make_func_export_array ();
#endif
      if (temp_array)
	add_temp_array_to_env (temp_array, 0, 0);

      array_needs_making = 0;
    }
}