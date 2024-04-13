execute_in_subshell (command, asynchronous, pipe_in, pipe_out, fds_to_close)
     COMMAND *command;
     int asynchronous;
     int pipe_in, pipe_out;
     struct fd_bitmap *fds_to_close;
{
  int user_subshell, return_code, function_value, should_redir_stdin, invert;
  int ois, user_coproc;
  int result;
  volatile COMMAND *tcom;

  USE_VAR(user_subshell);
  USE_VAR(user_coproc);
  USE_VAR(invert);
  USE_VAR(tcom);
  USE_VAR(asynchronous);

  subshell_level++;
  should_redir_stdin = (asynchronous && (command->flags & CMD_STDIN_REDIR) &&
			  pipe_in == NO_PIPE &&
			  stdin_redirects (command->redirects) == 0);

  invert = (command->flags & CMD_INVERT_RETURN) != 0;
  user_subshell = command->type == cm_subshell || ((command->flags & CMD_WANT_SUBSHELL) != 0);
  user_coproc = command->type == cm_coproc;

  command->flags &= ~(CMD_FORCE_SUBSHELL | CMD_WANT_SUBSHELL | CMD_INVERT_RETURN);

  /* If a command is asynchronous in a subshell (like ( foo ) & or
     the special case of an asynchronous GROUP command where the
     the subshell bit is turned on down in case cm_group: below),
     turn off `asynchronous', so that two subshells aren't spawned.
     XXX - asynchronous used to be set to 0 in this block, but that
     means that setup_async_signals was never run.  Now it's set to
     0 after subshell_environment is set appropriately and setup_async_signals
     is run.

     This seems semantically correct to me.  For example,
     ( foo ) & seems to say ``do the command `foo' in a subshell
     environment, but don't wait for that subshell to finish'',
     and "{ foo ; bar ; } &" seems to me to be like functions or
     builtins in the background, which executed in a subshell
     environment.  I just don't see the need to fork two subshells. */

  /* Don't fork again, we are already in a subshell.  A `doubly
     async' shell is not interactive, however. */
  if (asynchronous)
    {
#if defined (JOB_CONTROL)
      /* If a construct like ( exec xxx yyy ) & is given while job
	 control is active, we want to prevent exec from putting the
	 subshell back into the original process group, carefully
	 undoing all the work we just did in make_child. */
      original_pgrp = -1;
#endif /* JOB_CONTROL */
      ois = interactive_shell;
      interactive_shell = 0;
      /* This test is to prevent alias expansion by interactive shells that
	 run `(command) &' but to allow scripts that have enabled alias
	 expansion with `shopt -s expand_alias' to continue to expand
	 aliases. */
      if (ois != interactive_shell)
	expand_aliases = 0;
    }

  /* Subshells are neither login nor interactive. */
  login_shell = interactive = 0;

  if (user_subshell)
    subshell_environment = SUBSHELL_PAREN;
  else
    {
      subshell_environment = 0;			/* XXX */
      if (asynchronous)
	subshell_environment |= SUBSHELL_ASYNC;
      if (pipe_in != NO_PIPE || pipe_out != NO_PIPE)
	subshell_environment |= SUBSHELL_PIPE;
      if (user_coproc)
	subshell_environment |= SUBSHELL_COPROC;
    }

  reset_terminating_signals ();		/* in sig.c */
  /* Cancel traps, in trap.c. */
  /* Reset the signal handlers in the child, but don't free the
     trap strings.  Set a flag noting that we have to free the
     trap strings if we run trap to change a signal disposition. */
  reset_signal_handlers ();
  subshell_environment |= SUBSHELL_RESETTRAP;

  /* Make sure restore_original_signals doesn't undo the work done by
     make_child to ensure that asynchronous children are immune to SIGINT
     and SIGQUIT.  Turn off asynchronous to make sure more subshells are
     not spawned. */
  if (asynchronous)
    {
      setup_async_signals ();
      asynchronous = 0;
    }

#if defined (JOB_CONTROL)
  set_sigchld_handler ();
#endif /* JOB_CONTROL */

  set_sigint_handler ();

#if defined (JOB_CONTROL)
  /* Delete all traces that there were any jobs running.  This is
     only for subshells. */
  without_job_control ();
#endif /* JOB_CONTROL */

  if (fds_to_close)
    close_fd_bitmap (fds_to_close);

  do_piping (pipe_in, pipe_out);

#if defined (COPROCESS_SUPPORT)
  coproc_closeall ();
#endif

  /* If this is a user subshell, set a flag if stdin was redirected.
     This is used later to decide whether to redirect fd 0 to
     /dev/null for async commands in the subshell.  This adds more
     sh compatibility, but I'm not sure it's the right thing to do. */
  if (user_subshell)
    {
      stdin_redir = stdin_redirects (command->redirects);
      restore_default_signal (0);
    }

  /* If this is an asynchronous command (command &), we want to
     redirect the standard input from /dev/null in the absence of
     any specific redirection involving stdin. */
  if (should_redir_stdin && stdin_redir == 0)
    async_redirect_stdin ();

  /* Do redirections, then dispose of them before recursive call. */
  if (command->redirects)
    {
      if (do_redirections (command->redirects, RX_ACTIVE) != 0)
	exit (invert ? EXECUTION_SUCCESS : EXECUTION_FAILURE);

      dispose_redirects (command->redirects);
      command->redirects = (REDIRECT *)NULL;
    }

  if (command->type == cm_subshell)
    tcom = command->value.Subshell->command;
  else if (user_coproc)
    tcom = command->value.Coproc->command;
  else
    tcom = command;

  if (command->flags & CMD_TIME_PIPELINE)
    tcom->flags |= CMD_TIME_PIPELINE;
  if (command->flags & CMD_TIME_POSIX)
    tcom->flags |= CMD_TIME_POSIX;
  
  /* Make sure the subshell inherits any CMD_IGNORE_RETURN flag. */
  if ((command->flags & CMD_IGNORE_RETURN) && tcom != command)
    tcom->flags |= CMD_IGNORE_RETURN;

  /* If this is a simple command, tell execute_disk_command that it
     might be able to get away without forking and simply exec.
     This means things like ( sleep 10 ) will only cause one fork.
     If we're timing the command or inverting its return value, however,
     we cannot do this optimization. */
  if ((user_subshell || user_coproc) && (tcom->type == cm_simple || tcom->type == cm_subshell) &&
      ((tcom->flags & CMD_TIME_PIPELINE) == 0) &&
      ((tcom->flags & CMD_INVERT_RETURN) == 0))
    {
      tcom->flags |= CMD_NO_FORK;
      if (tcom->type == cm_simple)
	tcom->value.Simple->flags |= CMD_NO_FORK;
    }

  invert = (tcom->flags & CMD_INVERT_RETURN) != 0;
  tcom->flags &= ~CMD_INVERT_RETURN;

  result = setjmp (top_level);

  /* If we're inside a function while executing this subshell, we
     need to handle a possible `return'. */
  function_value = 0;
  if (return_catch_flag)
    function_value = setjmp (return_catch);

  /* If we're going to exit the shell, we don't want to invert the return
     status. */
  if (result == EXITPROG)
    invert = 0, return_code = last_command_exit_value;
  else if (result)
    return_code = EXECUTION_FAILURE;
  else if (function_value)
    return_code = return_catch_value;
  else
    return_code = execute_command_internal ((COMMAND *)tcom, asynchronous, NO_PIPE, NO_PIPE, fds_to_close);

  /* If we are asked to, invert the return value. */
  if (invert)
    return_code = (return_code == EXECUTION_SUCCESS) ? EXECUTION_FAILURE
						     : EXECUTION_SUCCESS;

  /* If we were explicitly placed in a subshell with (), we need
     to do the `shell cleanup' things, such as running traps[0]. */
  if (user_subshell && signal_is_trapped (0))
    {
      last_command_exit_value = return_code;
      return_code = run_exit_trap ();
    }

  subshell_level--;
  return (return_code);
  /* NOTREACHED */
}