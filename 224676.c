execute_function (var, words, flags, fds_to_close, async, subshell)
     SHELL_VAR *var;
     WORD_LIST *words;
     int flags;
     struct fd_bitmap *fds_to_close;
     int async, subshell;
{
  int return_val, result;
  COMMAND *tc, *fc, *save_current;
  char *debug_trap, *error_trap, *return_trap;
#if defined (ARRAY_VARS)
  SHELL_VAR *funcname_v, *nfv, *bash_source_v, *bash_lineno_v;
  ARRAY *funcname_a;
  volatile ARRAY *bash_source_a;
  volatile ARRAY *bash_lineno_a;
#endif
  FUNCTION_DEF *shell_fn;
  char *sfile, *t;

  USE_VAR(fc);

  if (funcnest_max > 0 && funcnest >= funcnest_max)
    {
      internal_error (_("%s: maximum function nesting level exceeded (%d)"), var->name, funcnest);
      funcnest = 0;	/* XXX - should we reset it somewhere else? */
      jump_to_top_level (DISCARD);
    }

#if defined (ARRAY_VARS)
  GET_ARRAY_FROM_VAR ("FUNCNAME", funcname_v, funcname_a);
  GET_ARRAY_FROM_VAR ("BASH_SOURCE", bash_source_v, bash_source_a);
  GET_ARRAY_FROM_VAR ("BASH_LINENO", bash_lineno_v, bash_lineno_a);
#endif

  tc = (COMMAND *)copy_command (function_cell (var));
  if (tc && (flags & CMD_IGNORE_RETURN))
    tc->flags |= CMD_IGNORE_RETURN;

  if (subshell == 0)
    {
      begin_unwind_frame ("function_calling");
      push_context (var->name, subshell, temporary_env);
      add_unwind_protect (pop_context, (char *)NULL);
      unwind_protect_int (line_number);
      unwind_protect_int (return_catch_flag);
      unwind_protect_jmp_buf (return_catch);
      add_unwind_protect (dispose_command, (char *)tc);
      unwind_protect_pointer (this_shell_function);
      unwind_protect_int (loop_level);
      unwind_protect_int (funcnest);
    }
  else
    push_context (var->name, subshell, temporary_env);	/* don't unwind-protect for subshells */

  temporary_env = (HASH_TABLE *)NULL;

  this_shell_function = var;
  make_funcname_visible (1);

  debug_trap = TRAP_STRING(DEBUG_TRAP);
  error_trap = TRAP_STRING(ERROR_TRAP);
  return_trap = TRAP_STRING(RETURN_TRAP);
  
  /* The order of the unwind protects for debug_trap, error_trap and
     return_trap is important here!  unwind-protect commands are run
     in reverse order of registration.  If this causes problems, take
     out the xfree unwind-protect calls and live with the small memory leak. */

  /* function_trace_mode != 0 means that all functions inherit the DEBUG trap.
     if the function has the trace attribute set, it inherits the DEBUG trap */
  if (debug_trap && ((trace_p (var) == 0) && function_trace_mode == 0))
    {
      if (subshell == 0)
	{
	  debug_trap = savestring (debug_trap);
	  add_unwind_protect (xfree, debug_trap);
	  add_unwind_protect (set_debug_trap, debug_trap);
	}
      restore_default_signal (DEBUG_TRAP);
    }

  /* error_trace_mode != 0 means that functions inherit the ERR trap. */
  if (error_trap && error_trace_mode == 0)
    {
      if (subshell == 0)
	{
	  error_trap = savestring (error_trap);
	  add_unwind_protect (xfree, error_trap);
	  add_unwind_protect (set_error_trap, error_trap);
	}
      restore_default_signal (ERROR_TRAP);
    }

  /* Shell functions inherit the RETURN trap if function tracing is on
     globally or on individually for this function. */
#if 0
  if (return_trap && ((trace_p (var) == 0) && function_trace_mode == 0))
#else
  if (return_trap && (signal_in_progress (DEBUG_TRAP) || ((trace_p (var) == 0) && function_trace_mode == 0)))
#endif
    {
      if (subshell == 0)
	{
	  return_trap = savestring (return_trap);
	  add_unwind_protect (xfree, return_trap);
	  add_unwind_protect (set_return_trap, return_trap);
	}
      restore_default_signal (RETURN_TRAP);
    }
  
  funcnest++;
#if defined (ARRAY_VARS)
  /* This is quite similar to the code in shell.c and elsewhere. */
  shell_fn = find_function_def (this_shell_function->name);
  sfile = shell_fn ? shell_fn->source_file : "";
  array_push ((ARRAY *)funcname_a, this_shell_function->name);

  array_push ((ARRAY *)bash_source_a, sfile);
  t = itos (executing_line_number ());
  array_push ((ARRAY *)bash_lineno_a, t);
  free (t);
#endif

  /* The temporary environment for a function is supposed to apply to
     all commands executed within the function body. */

  remember_args (words->next, 1);

  /* Update BASH_ARGV and BASH_ARGC */
  if (debugging_mode)
    push_args (words->next);

  /* Number of the line on which the function body starts. */
  line_number = function_line_number = tc->line;

#if defined (JOB_CONTROL)
  if (subshell)
    stop_pipeline (async, (COMMAND *)NULL);
#endif

  fc = tc;

  return_catch_flag++;
  return_val = setjmp (return_catch);

  if (return_val)
    {
      result = return_catch_value;
      /* Run the RETURN trap in the function's context. */
      save_current = currently_executing_command;
      run_return_trap ();
      currently_executing_command = save_current;
    }
  else
    {
      /* Run the debug trap here so we can trap at the start of a function's
	 execution rather than the execution of the body's first command. */
      showing_function_line = 1;
      save_current = currently_executing_command;
      result = run_debug_trap ();
#if defined (DEBUGGER)
      /* In debugging mode, if the DEBUG trap returns a non-zero status, we
	 skip the command. */
      if (debugging_mode == 0 || result == EXECUTION_SUCCESS)
	{
	  showing_function_line = 0;
	  currently_executing_command = save_current;
	  result = execute_command_internal (fc, 0, NO_PIPE, NO_PIPE, fds_to_close);

	  /* Run the RETURN trap in the function's context */
	  save_current = currently_executing_command;
	  run_return_trap ();
	  currently_executing_command = save_current;
	}
#else
      result = execute_command_internal (fc, 0, NO_PIPE, NO_PIPE, fds_to_close);

      save_current = currently_executing_command;
      run_return_trap ();
      currently_executing_command = save_current;
#endif
      showing_function_line = 0;
    }

  /* Restore BASH_ARGC and BASH_ARGV */
  if (debugging_mode)
    pop_args ();

  if (subshell == 0)
    run_unwind_frame ("function_calling");

#if defined (ARRAY_VARS)
  /* These two variables cannot be unset, and cannot be affected by the
     function. */
  array_pop ((ARRAY *)bash_source_a);
  array_pop ((ARRAY *)bash_lineno_a);

  /* FUNCNAME can be unset, and so can potentially be changed by the
     function. */
  GET_ARRAY_FROM_VAR ("FUNCNAME", nfv, funcname_a);
  if (nfv == funcname_v)
    array_pop (funcname_a);
#endif
  
  if (variable_context == 0 || this_shell_function == 0)
    {
      make_funcname_visible (0);
#if defined (PROCESS_SUBSTITUTION)
      unlink_fifo_list ();
#endif
    }
  
  return (result);
}