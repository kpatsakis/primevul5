execute_builtin_or_function (words, builtin, var, redirects,
			     fds_to_close, flags)
     WORD_LIST *words;
     sh_builtin_func_t *builtin;
     SHELL_VAR *var;
     REDIRECT *redirects;
     struct fd_bitmap *fds_to_close;
     int flags;
{
  int result;
  REDIRECT *saved_undo_list;
#if defined (PROCESS_SUBSTITUTION)
  int ofifo, nfifo, osize;
  char *ofifo_list;
#endif


#if defined (PROCESS_SUBSTITUTION)  
  ofifo = num_fifos ();
  ofifo_list = copy_fifo_list (&osize);
#endif

  if (do_redirections (redirects, RX_ACTIVE|RX_UNDOABLE) != 0)
    {
      cleanup_redirects (redirection_undo_list);
      redirection_undo_list = (REDIRECT *)NULL;
      dispose_exec_redirects ();
#if defined (PROCESS_SUBSTITUTION)
      free (ofifo_list);
#endif
      return (EX_REDIRFAIL);	/* was EXECUTION_FAILURE */
    }

  saved_undo_list = redirection_undo_list;

  /* Calling the "exec" builtin changes redirections forever. */
  if (builtin == exec_builtin)
    {
      dispose_redirects (saved_undo_list);
      saved_undo_list = exec_redirection_undo_list;
      exec_redirection_undo_list = (REDIRECT *)NULL;
    }
  else
    dispose_exec_redirects ();

  if (saved_undo_list)
    {
      begin_unwind_frame ("saved redirects");
      add_unwind_protect (cleanup_redirects, (char *)saved_undo_list);
    }

  redirection_undo_list = (REDIRECT *)NULL;

  if (builtin)
    result = execute_builtin (builtin, words, flags, 0);
  else
    result = execute_function (var, words, flags, fds_to_close, 0, 0);

  /* We do this before undoing the effects of any redirections. */
  fflush (stdout);
  fpurge (stdout);
  if (ferror (stdout))
    clearerr (stdout);  

  /* If we are executing the `command' builtin, but this_shell_builtin is
     set to `exec_builtin', we know that we have something like
     `command exec [redirection]', since otherwise `exec' would have
     overwritten the shell and we wouldn't get here.  In this case, we
     want to behave as if the `command' builtin had not been specified
     and preserve the redirections. */
  if (builtin == command_builtin && this_shell_builtin == exec_builtin)
    {
      int discard;

      discard = 0;
      if (saved_undo_list)
	{
	  dispose_redirects (saved_undo_list);
	  discard = 1;
	}
      redirection_undo_list = exec_redirection_undo_list;
      saved_undo_list = exec_redirection_undo_list = (REDIRECT *)NULL;      
      if (discard)
	discard_unwind_frame ("saved redirects");
    }

  if (saved_undo_list)
    {
      redirection_undo_list = saved_undo_list;
      discard_unwind_frame ("saved redirects");
    }

  if (redirection_undo_list)
    {
      cleanup_redirects (redirection_undo_list);
      redirection_undo_list = (REDIRECT *)NULL;
    }

#if defined (PROCESS_SUBSTITUTION)
  /* Close any FIFOs created by this builtin or function. */
  nfifo = num_fifos ();
  if (nfifo > ofifo)
    close_new_fifos (ofifo_list, osize);
  free (ofifo_list);
#endif

  return (result);
}