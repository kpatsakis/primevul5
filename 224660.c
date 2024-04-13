execute_intern_function (name, funcdef)
     WORD_DESC *name;
     FUNCTION_DEF *funcdef;
{
  SHELL_VAR *var;

  if (check_identifier (name, posixly_correct) == 0)
    {
      if (posixly_correct && interactive_shell == 0)
	{
	  last_command_exit_value = EX_BADUSAGE;
	  jump_to_top_level (ERREXIT);
	}
      return (EXECUTION_FAILURE);
    }

  /* Posix interpretation 383 */
  if (posixly_correct && find_special_builtin (name->word))
    {
      internal_error (_("`%s': is a special builtin"), name->word);
      last_command_exit_value = EX_BADUSAGE;
      jump_to_top_level (ERREXIT);
    }

  var = find_function (name->word);
  if (var && (readonly_p (var) || noassign_p (var)))
    {
      if (readonly_p (var))
	internal_error (_("%s: readonly function"), var->name);
      return (EXECUTION_FAILURE);
    }

#if defined (DEBUGGER)
  bind_function_def (name->word, funcdef);
#endif

  bind_function (name->word, funcdef->command);
  return (EXECUTION_SUCCESS);
}