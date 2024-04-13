do_piping (pipe_in, pipe_out)
     int pipe_in, pipe_out;
{
  if (pipe_in != NO_PIPE)
    {
      if (dup2 (pipe_in, 0) < 0)
	dup_error (pipe_in, 0);
      if (pipe_in > 0)
	close (pipe_in);
#ifdef __CYGWIN__
      /* Let stdio know the fd may have changed from text to binary mode. */
      freopen (NULL, "r", stdin);
#endif /* __CYGWIN__ */
    }
  if (pipe_out != NO_PIPE)
    {
      if (pipe_out != REDIRECT_BOTH)
	{
	  if (dup2 (pipe_out, 1) < 0)
	    dup_error (pipe_out, 1);
	  if (pipe_out == 0 || pipe_out > 1)
	    close (pipe_out);
	}
      else
	{
	  if (dup2 (1, 2) < 0)
	    dup_error (1, 2);
	}
#ifdef __CYGWIN__
      /* Let stdio know the fd may have changed from text to binary mode, and
	 make sure to preserve stdout line buffering. */
      freopen (NULL, "w", stdout);
      sh_setlinebuf (stdout);
#endif /* __CYGWIN__ */
    }
}