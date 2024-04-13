execute_null_command (redirects, pipe_in, pipe_out, async)
     REDIRECT *redirects;
     int pipe_in, pipe_out, async;
{
  int r;
  int forcefork;
  REDIRECT *rd;

  for (forcefork = 0, rd = redirects; rd; rd = rd->next)
    forcefork += rd->rflags & REDIR_VARASSIGN;

  if (forcefork || pipe_in != NO_PIPE || pipe_out != NO_PIPE || async)
    {
      /* We have a null command, but we really want a subshell to take
	 care of it.  Just fork, do piping and redirections, and exit. */
      if (make_child ((char *)NULL, async) == 0)
	{
	  /* Cancel traps, in trap.c. */
	  restore_original_signals ();		/* XXX */

	  do_piping (pipe_in, pipe_out);

#if defined (COPROCESS_SUPPORT)
	  coproc_closeall ();
#endif

	  subshell_environment = 0;
	  if (async)
	    subshell_environment |= SUBSHELL_ASYNC;
	  if (pipe_in != NO_PIPE || pipe_out != NO_PIPE)
	    subshell_environment |= SUBSHELL_PIPE;

	  if (do_redirections (redirects, RX_ACTIVE) == 0)
	    exit (EXECUTION_SUCCESS);
	  else
	    exit (EXECUTION_FAILURE);
	}
      else
	{
	  close_pipes (pipe_in, pipe_out);
#if defined (PROCESS_SUBSTITUTION) && defined (HAVE_DEV_FD)
	  unlink_fifo_list ();
#endif
	  return (EXECUTION_SUCCESS);
	}
    }
  else
    {
      /* Even if there aren't any command names, pretend to do the
	 redirections that are specified.  The user expects the side
	 effects to take place.  If the redirections fail, then return
	 failure.  Otherwise, if a command substitution took place while
	 expanding the command or a redirection, return the value of that
	 substitution.  Otherwise, return EXECUTION_SUCCESS. */

      r = do_redirections (redirects, RX_ACTIVE|RX_UNDOABLE);
      cleanup_redirects (redirection_undo_list);
      redirection_undo_list = (REDIRECT *)NULL;

      if (r != 0)
	return (EXECUTION_FAILURE);
      else if (last_command_subst_pid != NO_PID)
	return (last_command_exit_value);
      else
	return (EXECUTION_SUCCESS);
    }
}