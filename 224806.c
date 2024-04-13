execute_subshell_builtin_or_function (words, redirects, builtin, var,
				      pipe_in, pipe_out, async, fds_to_close,
				      flags)
     WORD_LIST *words;
     REDIRECT *redirects;
     sh_builtin_func_t *builtin;
     SHELL_VAR *var;
     int pipe_in, pipe_out, async;
     struct fd_bitmap *fds_to_close;
     int flags;
{
  int result, r, funcvalue;
#if defined (JOB_CONTROL)
  int jobs_hack;

  jobs_hack = (builtin == jobs_builtin) &&
		((subshell_environment & SUBSHELL_ASYNC) == 0 || pipe_out != NO_PIPE);
#endif

  /* A subshell is neither a login shell nor interactive. */
  login_shell = interactive = 0;

  if (async)
    subshell_environment |= SUBSHELL_ASYNC;
  if (pipe_in != NO_PIPE || pipe_out != NO_PIPE)
    subshell_environment |= SUBSHELL_PIPE;

  maybe_make_export_env ();	/* XXX - is this needed? */

#if defined (JOB_CONTROL)
  /* Eradicate all traces of job control after we fork the subshell, so
     all jobs begun by this subshell are in the same process group as
     the shell itself. */

  /* Allow the output of `jobs' to be piped. */
  if (jobs_hack)
    kill_current_pipeline ();
  else
    without_job_control ();

  set_sigchld_handler ();
#endif /* JOB_CONTROL */

  set_sigint_handler ();

  if (fds_to_close)
    close_fd_bitmap (fds_to_close);

  do_piping (pipe_in, pipe_out);

  if (do_redirections (redirects, RX_ACTIVE) != 0)
    exit (EXECUTION_FAILURE);

  if (builtin)
    {
      /* Give builtins a place to jump back to on failure,
	 so we don't go back up to main(). */
      result = setjmp (top_level);

      /* Give the return builtin a place to jump to when executed in a subshell
         or pipeline */
      funcvalue = 0;
      if (return_catch_flag && builtin == return_builtin)
        funcvalue = setjmp (return_catch);

      if (result == EXITPROG)
	exit (last_command_exit_value);
      else if (result)
	exit (EXECUTION_FAILURE);
      else if (funcvalue)
	exit (return_catch_value);
      else
	{
	  r = execute_builtin (builtin, words, flags, 1);
	  fflush (stdout);
	  if (r == EX_USAGE)
	    r = EX_BADUSAGE;
	  exit (r);
	}
    }
  else
    {
      r = execute_function (var, words, flags, fds_to_close, async, 1);
      fflush (stdout);
      exit (r);
    }
}