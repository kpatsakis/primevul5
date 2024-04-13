execute_connection (command, asynchronous, pipe_in, pipe_out, fds_to_close)
     COMMAND *command;
     int asynchronous, pipe_in, pipe_out;
     struct fd_bitmap *fds_to_close;
{
  COMMAND *tc, *second;
  int ignore_return, exec_result, was_error_trap, invert;
  volatile int save_line_number;

  ignore_return = (command->flags & CMD_IGNORE_RETURN) != 0;

  switch (command->value.Connection->connector)
    {
    /* Do the first command asynchronously. */
    case '&':
      tc = command->value.Connection->first;
      if (tc == 0)
	return (EXECUTION_SUCCESS);

      if (ignore_return)
	tc->flags |= CMD_IGNORE_RETURN;
      tc->flags |= CMD_AMPERSAND;

      /* If this shell was compiled without job control support,
	 if we are currently in a subshell via `( xxx )', or if job
	 control is not active then the standard input for an
	 asynchronous command is forced to /dev/null. */
#if defined (JOB_CONTROL)
      if ((subshell_environment || !job_control) && !stdin_redir)
#else
      if (!stdin_redir)
#endif /* JOB_CONTROL */
	tc->flags |= CMD_STDIN_REDIR;

      exec_result = execute_command_internal (tc, 1, pipe_in, pipe_out, fds_to_close);
      QUIT;

      if (tc->flags & CMD_STDIN_REDIR)
	tc->flags &= ~CMD_STDIN_REDIR;

      second = command->value.Connection->second;
      if (second)
	{
	  if (ignore_return)
	    second->flags |= CMD_IGNORE_RETURN;

	  exec_result = execute_command_internal (second, asynchronous, pipe_in, pipe_out, fds_to_close);
	}

      break;

    /* Just call execute command on both sides. */
    case ';':
      if (ignore_return)
	{
	  if (command->value.Connection->first)
	    command->value.Connection->first->flags |= CMD_IGNORE_RETURN;
	  if (command->value.Connection->second)
	    command->value.Connection->second->flags |= CMD_IGNORE_RETURN;
	}
      executing_list++;
      QUIT;
      execute_command (command->value.Connection->first);
      QUIT;
      exec_result = execute_command_internal (command->value.Connection->second,
				      asynchronous, pipe_in, pipe_out,
				      fds_to_close);
      executing_list--;
      break;

    case '|':
      was_error_trap = signal_is_trapped (ERROR_TRAP) && signal_is_ignored (ERROR_TRAP) == 0;
      invert = (command->flags & CMD_INVERT_RETURN) != 0;
      ignore_return = (command->flags & CMD_IGNORE_RETURN) != 0;

      line_number_for_err_trap = line_number;
      exec_result = execute_pipeline (command, asynchronous, pipe_in, pipe_out, fds_to_close);

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

    case AND_AND:
    case OR_OR:
      if (asynchronous)
	{
	  /* If we have something like `a && b &' or `a || b &', run the
	     && or || stuff in a subshell.  Force a subshell and just call
	     execute_command_internal again.  Leave asynchronous on
	     so that we get a report from the parent shell about the
	     background job. */
	  command->flags |= CMD_FORCE_SUBSHELL;
	  exec_result = execute_command_internal (command, 1, pipe_in, pipe_out, fds_to_close);
	  break;
	}

      /* Execute the first command.  If the result of that is successful
	 and the connector is AND_AND, or the result is not successful
	 and the connector is OR_OR, then execute the second command,
	 otherwise return. */

      executing_list++;
      if (command->value.Connection->first)
	command->value.Connection->first->flags |= CMD_IGNORE_RETURN;

      exec_result = execute_command (command->value.Connection->first);
      QUIT;
      if (((command->value.Connection->connector == AND_AND) &&
	   (exec_result == EXECUTION_SUCCESS)) ||
	  ((command->value.Connection->connector == OR_OR) &&
	   (exec_result != EXECUTION_SUCCESS)))
	{
	  if (ignore_return && command->value.Connection->second)
	    command->value.Connection->second->flags |= CMD_IGNORE_RETURN;

	  exec_result = execute_command (command->value.Connection->second);
	}
      executing_list--;
      break;

    default:
      command_error ("execute_connection", CMDERR_BADCONN, command->value.Connection->connector, 0);
      jump_to_top_level (DISCARD);
      exec_result = EXECUTION_FAILURE;
    }

  return exec_result;
}