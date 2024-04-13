execute_builtin (builtin, words, flags, subshell)
     sh_builtin_func_t *builtin;
     WORD_LIST *words;
     int flags, subshell;
{
  int old_e_flag, result, eval_unwind;
  int isbltinenv;
  char *error_trap;

  error_trap = 0;
  old_e_flag = exit_immediately_on_error;
  /* The eval builtin calls parse_and_execute, which does not know about
     the setting of flags, and always calls the execution functions with
     flags that will exit the shell on an error if -e is set.  If the
     eval builtin is being called, and we're supposed to ignore the exit
     value of the command, we turn the -e flag off ourselves and disable
     the ERR trap, then restore them when the command completes.  This is
     also a problem (as below) for the command and source/. builtins. */
  if (subshell == 0 && (flags & CMD_IGNORE_RETURN) &&
	(builtin == eval_builtin || builtin == command_builtin || builtin == source_builtin))
    {
      begin_unwind_frame ("eval_builtin");
      unwind_protect_int (exit_immediately_on_error);
      error_trap = TRAP_STRING (ERROR_TRAP);
      if (error_trap)
	{
	  error_trap = savestring (error_trap);
	  add_unwind_protect (xfree, error_trap);
	  add_unwind_protect (set_error_trap, error_trap);
	  restore_default_signal (ERROR_TRAP);
	}
      exit_immediately_on_error = 0;
      eval_unwind = 1;
    }
  else
    eval_unwind = 0;

  /* The temporary environment for a builtin is supposed to apply to
     all commands executed by that builtin.  Currently, this is a
     problem only with the `unset', `source' and `eval' builtins. */

  isbltinenv = (builtin == source_builtin || builtin == eval_builtin || builtin == unset_builtin);

  if (isbltinenv)
    {
      if (subshell == 0)
	begin_unwind_frame ("builtin_env");

      if (temporary_env)
	{
	  push_scope (VC_BLTNENV, temporary_env);
	  if (subshell == 0)
	    add_unwind_protect (pop_scope, (flags & CMD_COMMAND_BUILTIN) ? 0 : "1");
          temporary_env = (HASH_TABLE *)NULL;	  
	}
    }

  /* `return' does a longjmp() back to a saved environment in execute_function.
     If a variable assignment list preceded the command, and the shell is
     running in POSIX mode, we need to merge that into the shell_variables
     table, since `return' is a POSIX special builtin. */
  if (posixly_correct && subshell == 0 && builtin == return_builtin && temporary_env)
    {
      begin_unwind_frame ("return_temp_env");
      add_unwind_protect (merge_temporary_env, (char *)NULL);
    }

  executing_builtin++;
  executing_command_builtin |= builtin == command_builtin;
  result = ((*builtin) (words->next));

  /* This shouldn't happen, but in case `return' comes back instead of
     longjmp'ing, we need to unwind. */
  if (posixly_correct && subshell == 0 && builtin == return_builtin && temporary_env)
    discard_unwind_frame ("return_temp_env");

  if (subshell == 0 && isbltinenv)
    run_unwind_frame ("builtin_env");

  if (eval_unwind)
    {
      exit_immediately_on_error += old_e_flag;
      if (error_trap)
	{
	  set_error_trap (error_trap);
	  xfree (error_trap);
	}
      discard_unwind_frame ("eval_builtin");
    }

  return (result);
}