execute_coproc (command, pipe_in, pipe_out, fds_to_close)
     COMMAND *command;
     int pipe_in, pipe_out;
     struct fd_bitmap *fds_to_close;
{
  int rpipe[2], wpipe[2], estat;
  pid_t coproc_pid;
  Coproc *cp;
  char *tcmd;

  /* XXX -- can be removed after changes to handle multiple coprocs */
#if !MULTIPLE_COPROCS
  if (sh_coproc.c_pid != NO_PID)
    internal_warning ("execute_coproc: coproc [%d:%s] still exists", sh_coproc.c_pid, sh_coproc.c_name);
  coproc_init (&sh_coproc);
#endif

  command_string_index = 0;
  tcmd = make_command_string (command);

  sh_openpipe ((int *)&rpipe);	/* 0 = parent read, 1 = child write */
  sh_openpipe ((int *)&wpipe); /* 0 = child read, 1 = parent write */

  coproc_pid = make_child (savestring (tcmd), 1);
  if (coproc_pid == 0)
    {
      close (rpipe[0]);
      close (wpipe[1]);

      estat = execute_in_subshell (command, 1, wpipe[0], rpipe[1], fds_to_close);

      fflush (stdout);
      fflush (stderr);

      exit (estat);
    }

  close (rpipe[1]);
  close (wpipe[0]);

  cp = coproc_alloc (command->value.Coproc->name, coproc_pid);
  cp->c_rfd = rpipe[0];
  cp->c_wfd = wpipe[1];

  SET_CLOSE_ON_EXEC (cp->c_rfd);
  SET_CLOSE_ON_EXEC (cp->c_wfd);

  coproc_setvars (cp);

#if 0
  itrace ("execute_coproc: [%d] %s", coproc_pid, the_printed_command);
#endif

  close_pipes (pipe_in, pipe_out);
#if defined (PROCESS_SUBSTITUTION) && defined (HAVE_DEV_FD)
  unlink_fifo_list ();
#endif
  stop_pipeline (1, (COMMAND *)NULL);
  DESCRIBE_PID (coproc_pid);
  run_pending_traps ();

  return (EXECUTION_SUCCESS);
}