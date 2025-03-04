execute_command_internal (command, asynchronous, pipe_in, pipe_out,
			  fds_to_close)
     COMMAND *command;
     int asynchronous;
     int pipe_in, pipe_out;
     struct fd_bitmap *fds_to_close;
{
  int exec_result, user_subshell, invert, ignore_return, was_error_trap;
  REDIRECT *my_undo_list, *exec_undo_list;
  volatile int last_pid;
  volatile int save_line_number;
#if defined (PROCESS_SUBSTITUTION)
  volatile int ofifo, nfifo, osize, saved_fifo;
  volatile char *ofifo_list;
#endif

#if 0
  if (command == 0 || breaking || continuing || read_but_dont_execute)
    return (EXECUTION_SUCCESS);
#else
  if (breaking || continuing)
    return (last_command_exit_value);
  if (command == 0 || read_but_dont_execute)
    return (EXECUTION_SUCCESS);
#endif

  QUIT;
  run_pending_traps ();

#if 0
  if (running_trap == 0)
#endif
    currently_executing_command = command;

  invert = (command->flags & CMD_INVERT_RETURN) != 0;

  /* If we're inverting the return value and `set -e' has been executed,
     we don't want a failing command to inadvertently cause the shell
     to exit. */
  if (exit_immediately_on_error && invert)	/* XXX */
    command->flags |= CMD_IGNORE_RETURN;	/* XXX */

  exec_result = EXECUTION_SUCCESS;

  /* If a command was being explicitly run in a subshell, or if it is
     a shell control-structure, and it has a pipe, then we do the command
     in a subshell. */
  if (command->type == cm_subshell && (command->flags & CMD_NO_FORK))
    return (execute_in_subshell (command, asynchronous, pipe_in, pipe_out, fds_to_close));

#if defined (COPROCESS_SUPPORT)
  if (command->type == cm_coproc)
    return (execute_coproc (command, pipe_in, pipe_out, fds_to_close));
#endif

  user_subshell = command->type == cm_subshell || ((command->flags & CMD_WANT_SUBSHELL) != 0);

  if (command->type == cm_subshell ||
      (command->flags & (CMD_WANT_SUBSHELL|CMD_FORCE_SUBSHELL)) ||
      (shell_control_structure (command->type) &&
       (pipe_out != NO_PIPE || pipe_in != NO_PIPE || asynchronous)))
    {
      pid_t paren_pid;

      /* Fork a subshell, turn off the subshell bit, turn off job
	 control and call execute_command () on the command again. */
      line_number_for_err_trap = line_number;
      paren_pid = make_child (savestring (make_command_string (command)),
			      asynchronous);
      if (paren_pid == 0)
	exit (execute_in_subshell (command, asynchronous, pipe_in, pipe_out, fds_to_close));
	/* NOTREACHED */
      else
	{
	  close_pipes (pipe_in, pipe_out);

#if defined (PROCESS_SUBSTITUTION) && defined (HAVE_DEV_FD)
	  if (variable_context == 0)	/* wait until shell function completes */
	    unlink_fifo_list ();
#endif
	  /* If we are part of a pipeline, and not the end of the pipeline,
	     then we should simply return and let the last command in the
	     pipe be waited for.  If we are not in a pipeline, or are the
	     last command in the pipeline, then we wait for the subshell
	     and return its exit status as usual. */
	  if (pipe_out != NO_PIPE)
	    return (EXECUTION_SUCCESS);

	  stop_pipeline (asynchronous, (COMMAND *)NULL);

	  if (asynchronous == 0)
	    {
	      was_error_trap = signal_is_trapped (ERROR_TRAP) && signal_is_ignored (ERROR_TRAP) == 0;
	      invert = (command->flags & CMD_INVERT_RETURN) != 0;
	      ignore_return = (command->flags & CMD_IGNORE_RETURN) != 0;

	      exec_result = wait_for (paren_pid);

	      /* If we have to, invert the return value. */
	      if (invert)
		exec_result = ((exec_result == EXECUTION_SUCCESS)
				? EXECUTION_FAILURE
				: EXECUTION_SUCCESS);

	      last_command_exit_value = exec_result;
	      if (user_subshell && was_error_trap && ignore_return == 0 && invert == 0 && exec_result != EXECUTION_SUCCESS)
		{
		  save_line_number = line_number;
		  line_number = line_number_for_err_trap;
		  run_error_trap ();
		  line_number = save_line_number;
		}

	      if (user_subshell && ignore_return == 0 && invert == 0 && exit_immediately_on_error && exec_result != EXECUTION_SUCCESS)
		{
		  run_pending_traps ();
		  jump_to_top_level (ERREXIT);
		}

	      return (last_command_exit_value);
	    }
	  else
	    {
	      DESCRIBE_PID (paren_pid);

	      run_pending_traps ();

	      return (EXECUTION_SUCCESS);
	    }
	}
    }

#if defined (COMMAND_TIMING)
  if (command->flags & CMD_TIME_PIPELINE)
    {
      if (asynchronous)
	{
	  command->flags |= CMD_FORCE_SUBSHELL;
	  exec_result = execute_command_internal (command, 1, pipe_in, pipe_out, fds_to_close);
	}
      else
	{
	  exec_result = time_command (command, asynchronous, pipe_in, pipe_out, fds_to_close);
#if 0
	  if (running_trap == 0)
#endif
	    currently_executing_command = (COMMAND *)NULL;
	}
      return (exec_result);
    }
#endif /* COMMAND_TIMING */

  if (shell_control_structure (command->type) && command->redirects)
    stdin_redir = stdin_redirects (command->redirects);

#if defined (PROCESS_SUBSTITUTION)
  if (variable_context != 0)
    {
      ofifo = num_fifos ();
      ofifo_list = copy_fifo_list (&osize);
      saved_fifo = 1;
    }
  else
    saved_fifo = 0;
#endif

  /* Handle WHILE FOR CASE etc. with redirections.  (Also '&' input
     redirection.)  */
  if (do_redirections (command->redirects, RX_ACTIVE|RX_UNDOABLE) != 0)
    {
      cleanup_redirects (redirection_undo_list);
      redirection_undo_list = (REDIRECT *)NULL;
      dispose_exec_redirects ();
#if defined (PROCESS_SUBSTITUTION)
      if (saved_fifo)
	free (ofifo_list);
#endif
      return (last_command_exit_value = EXECUTION_FAILURE);
    }

  if (redirection_undo_list)
    {
      /* XXX - why copy here? */
      my_undo_list = (REDIRECT *)copy_redirects (redirection_undo_list);
      dispose_redirects (redirection_undo_list);
      redirection_undo_list = (REDIRECT *)NULL;
    }
  else
    my_undo_list = (REDIRECT *)NULL;

  if (exec_redirection_undo_list)
    {
      /* XXX - why copy here? */
      exec_undo_list = (REDIRECT *)copy_redirects (exec_redirection_undo_list);
      dispose_redirects (exec_redirection_undo_list);
      exec_redirection_undo_list = (REDIRECT *)NULL;
    }
  else
    exec_undo_list = (REDIRECT *)NULL;

  if (my_undo_list || exec_undo_list)
    begin_unwind_frame ("loop_redirections");

  if (my_undo_list)
    add_unwind_protect ((Function *)cleanup_redirects, my_undo_list);

  if (exec_undo_list)
    add_unwind_protect ((Function *)dispose_redirects, exec_undo_list);

  ignore_return = (command->flags & CMD_IGNORE_RETURN) != 0;

  QUIT;

  switch (command->type)
    {
    case cm_simple:
      {
	save_line_number = line_number;
	/* We can't rely on variables retaining their values across a
	   call to execute_simple_command if a longjmp occurs as the
	   result of a `return' builtin.  This is true for sure with gcc. */
#if defined (RECYCLES_PIDS)
	last_made_pid = NO_PID;
#endif
	last_pid = last_made_pid;
	was_error_trap = signal_is_trapped (ERROR_TRAP) && signal_is_ignored (ERROR_TRAP) == 0;

	if (ignore_return && command->value.Simple)
	  command->value.Simple->flags |= CMD_IGNORE_RETURN;
	if (command->flags & CMD_STDIN_REDIR)
	  command->value.Simple->flags |= CMD_STDIN_REDIR;

	line_number_for_err_trap = line_number = command->value.Simple->line;
	exec_result =
	  execute_simple_command (command->value.Simple, pipe_in, pipe_out,
				  asynchronous, fds_to_close);
	line_number = save_line_number;

	/* The temporary environment should be used for only the simple
	   command immediately following its definition. */
	dispose_used_env_vars ();

#if (defined (ultrix) && defined (mips)) || defined (C_ALLOCA)
	/* Reclaim memory allocated with alloca () on machines which
	   may be using the alloca emulation code. */
	(void) alloca (0);
#endif /* (ultrix && mips) || C_ALLOCA */

	/* If we forked to do the command, then we must wait_for ()
	   the child. */

	/* XXX - this is something to watch out for if there are problems
	   when the shell is compiled without job control. */
	if (already_making_children && pipe_out == NO_PIPE &&
	    last_made_pid != last_pid)
	  {
	    stop_pipeline (asynchronous, (COMMAND *)NULL);

	    if (asynchronous)
	      {
		DESCRIBE_PID (last_made_pid);
	      }
	    else
#if !defined (JOB_CONTROL)
	      /* Do not wait for asynchronous processes started from
		 startup files. */
	    if (last_made_pid != last_asynchronous_pid)
#endif
	    /* When executing a shell function that executes other
	       commands, this causes the last simple command in
	       the function to be waited for twice.  This also causes
	       subshells forked to execute builtin commands (e.g., in
	       pipelines) to be waited for twice. */
	      exec_result = wait_for (last_made_pid);
	  }
      }

      /* 2009/02/13 -- pipeline failure is processed elsewhere.  This handles
	 only the failure of a simple command. */
      if (was_error_trap && ignore_return == 0 && invert == 0 && pipe_in == NO_PIPE && pipe_out == NO_PIPE && exec_result != EXECUTION_SUCCESS)
	{
	  last_command_exit_value = exec_result;
	  line_number = line_number_for_err_trap;
	  run_error_trap ();
	  line_number = save_line_number;
	}

      if (ignore_return == 0 && invert == 0 &&
	  ((posixly_correct && interactive == 0 && special_builtin_failed) ||
	   (exit_immediately_on_error && pipe_in == NO_PIPE && pipe_out == NO_PIPE && exec_result != EXECUTION_SUCCESS)))
	{
	  last_command_exit_value = exec_result;
	  run_pending_traps ();
	  jump_to_top_level (ERREXIT);
	}

      break;

    case cm_for:
      if (ignore_return)
	command->value.For->flags |= CMD_IGNORE_RETURN;
      exec_result = execute_for_command (command->value.For);
      break;

#if defined (ARITH_FOR_COMMAND)
    case cm_arith_for:
      if (ignore_return)
	command->value.ArithFor->flags |= CMD_IGNORE_RETURN;
      exec_result = execute_arith_for_command (command->value.ArithFor);
      break;
#endif

#if defined (SELECT_COMMAND)
    case cm_select:
      if (ignore_return)
	command->value.Select->flags |= CMD_IGNORE_RETURN;
      exec_result = execute_select_command (command->value.Select);
      break;
#endif

    case cm_case:
      if (ignore_return)
	command->value.Case->flags |= CMD_IGNORE_RETURN;
      exec_result = execute_case_command (command->value.Case);
      break;

    case cm_while:
      if (ignore_return)
	command->value.While->flags |= CMD_IGNORE_RETURN;
      exec_result = execute_while_command (command->value.While);
      break;

    case cm_until:
      if (ignore_return)
	command->value.While->flags |= CMD_IGNORE_RETURN;
      exec_result = execute_until_command (command->value.While);
      break;

    case cm_if:
      if (ignore_return)
	command->value.If->flags |= CMD_IGNORE_RETURN;
      exec_result = execute_if_command (command->value.If);
      break;

    case cm_group:

      /* This code can be executed from either of two paths: an explicit
	 '{}' command, or via a function call.  If we are executed via a
	 function call, we have already taken care of the function being
	 executed in the background (down there in execute_simple_command ()),
	 and this command should *not* be marked as asynchronous.  If we
	 are executing a regular '{}' group command, and asynchronous == 1,
	 we must want to execute the whole command in the background, so we
	 need a subshell, and we want the stuff executed in that subshell
	 (this group command) to be executed in the foreground of that
	 subshell (i.e. there will not be *another* subshell forked).

	 What we do is to force a subshell if asynchronous, and then call
	 execute_command_internal again with asynchronous still set to 1,
	 but with the original group command, so the printed command will
	 look right.

	 The code above that handles forking off subshells will note that
	 both subshell and async are on, and turn off async in the child
	 after forking the subshell (but leave async set in the parent, so
	 the normal call to describe_pid is made).  This turning off
	 async is *crucial*; if it is not done, this will fall into an
	 infinite loop of executions through this spot in subshell after
	 subshell until the process limit is exhausted. */

      if (asynchronous)
	{
	  command->flags |= CMD_FORCE_SUBSHELL;
	  exec_result =
	    execute_command_internal (command, 1, pipe_in, pipe_out,
				      fds_to_close);
	}
      else
	{
	  if (ignore_return && command->value.Group->command)
	    command->value.Group->command->flags |= CMD_IGNORE_RETURN;
	  exec_result =
	    execute_command_internal (command->value.Group->command,
				      asynchronous, pipe_in, pipe_out,
				      fds_to_close);
	}
      break;

    case cm_connection:
      exec_result = execute_connection (command, asynchronous,
					pipe_in, pipe_out, fds_to_close);
      break;

#if defined (DPAREN_ARITHMETIC)
    case cm_arith:
      was_error_trap = signal_is_trapped (ERROR_TRAP) && signal_is_ignored (ERROR_TRAP) == 0;
      if (ignore_return)
	command->value.Arith->flags |= CMD_IGNORE_RETURN;
      line_number_for_err_trap = save_line_number = line_number;
      exec_result = execute_arith_command (command->value.Arith);
      line_number = save_line_number;

      if (was_error_trap && ignore_return == 0 && invert == 0 && exec_result != EXECUTION_SUCCESS)
	{
	  last_command_exit_value = exec_result;
	  save_line_number = line_number;
	  line_number = line_number_for_err_trap;
	  run_error_trap ();
	  line_number = save_line_number;
	}

      if (ignore_return == 0 && invert == 0 && exit_immediately_on_error && exec_result != EXECUTION_SUCCESS)
	{
	  last_command_exit_value = exec_result;
	  run_pending_traps ();
	  jump_to_top_level (ERREXIT);
	}

      break;
#endif

#if defined (COND_COMMAND)
    case cm_cond:
      was_error_trap = signal_is_trapped (ERROR_TRAP) && signal_is_ignored (ERROR_TRAP) == 0;
      if (ignore_return)
	command->value.Cond->flags |= CMD_IGNORE_RETURN;

      line_number_for_err_trap = save_line_number = line_number;
      exec_result = execute_cond_command (command->value.Cond);
      line_number = save_line_number;

      if (was_error_trap && ignore_return == 0 && invert == 0 && exec_result != EXECUTION_SUCCESS)
	{
	  last_command_exit_value = exec_result;
	  save_line_number = line_number;
	  line_number = line_number_for_err_trap;
	  run_error_trap ();
	  line_number = save_line_number;
	}

      if (ignore_return == 0 && invert == 0 && exit_immediately_on_error && exec_result != EXECUTION_SUCCESS)
	{
	  last_command_exit_value = exec_result;
	  run_pending_traps ();
	  jump_to_top_level (ERREXIT);
	}

      break;
#endif
    
    case cm_function_def:
      exec_result = execute_intern_function (command->value.Function_def->name,
					     command->value.Function_def);
      break;

    default:
      command_error ("execute_command", CMDERR_BADTYPE, command->type, 0);
    }

  if (my_undo_list)
    {
      do_redirections (my_undo_list, RX_ACTIVE);
      dispose_redirects (my_undo_list);
    }

  if (exec_undo_list)
    dispose_redirects (exec_undo_list);

  if (my_undo_list || exec_undo_list)
    discard_unwind_frame ("loop_redirections");

#if defined (PROCESS_SUBSTITUTION)
  if (saved_fifo)
    {
      nfifo = num_fifos ();
      if (nfifo > ofifo)
	close_new_fifos (ofifo_list, osize);
      free (ofifo_list);
    }
#endif

  /* Invert the return value if we have to */
  if (invert)
    exec_result = (exec_result == EXECUTION_SUCCESS)
		    ? EXECUTION_FAILURE
		    : EXECUTION_SUCCESS;

#if defined (DPAREN_ARITHMETIC) || defined (COND_COMMAND)
  /* This is where we set PIPESTATUS from the exit status of the appropriate
     compound commands (the ones that look enough like simple commands to
     cause confusion).  We might be able to optimize by not doing this if
     subshell_environment != 0. */
  switch (command->type)
    {
#  if defined (DPAREN_ARITHMETIC)
    case cm_arith:
#  endif
#  if defined (COND_COMMAND)
    case cm_cond:
#  endif
      set_pipestatus_from_exit (exec_result);
      break;
    }
#endif

  last_command_exit_value = exec_result;
  run_pending_traps ();
#if 0
  if (running_trap == 0)
#endif
    currently_executing_command = (COMMAND *)NULL;

  return (last_command_exit_value);
}