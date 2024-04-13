shell_execve (command, args, env)
     char *command;
     char **args, **env;
{
  int larray, i, fd;
  char sample[80];
  int sample_len;

  SETOSTYPE (0);		/* Some systems use for USG/POSIX semantics */
  execve (command, args, env);
  i = errno;			/* error from execve() */
  CHECK_TERMSIG;
  SETOSTYPE (1);

  /* If we get to this point, then start checking out the file.
     Maybe it is something we can hack ourselves. */
  if (i != ENOEXEC)
    {
      if (file_isdir (command))
#if defined (EISDIR)
	internal_error (_("%s: %s"), command, strerror (EISDIR));
#else
	internal_error (_("%s: is a directory"), command);
#endif
      else if (executable_file (command) == 0)
	{
	  errno = i;
	  file_error (command);
	}
      /* errors not involving the path argument to execve. */
      else if (i == E2BIG || i == ENOMEM)
	{
	  errno = i;
	  file_error (command);
	}
      else
	{
	  /* The file has the execute bits set, but the kernel refuses to
	     run it for some reason.  See why. */
#if defined (HAVE_HASH_BANG_EXEC)
	  READ_SAMPLE_BUF (command, sample, sample_len);
	  sample[sample_len - 1] = '\0';
	  if (sample_len > 2 && sample[0] == '#' && sample[1] == '!')
	    {
	      char *interp;
	      int ilen;

	      interp = getinterp (sample, sample_len, (int *)NULL);
	      ilen = strlen (interp);
	      errno = i;
	      if (interp[ilen - 1] == '\r')
		{
		  interp = xrealloc (interp, ilen + 2);
		  interp[ilen - 1] = '^';
		  interp[ilen] = 'M';
		  interp[ilen + 1] = '\0';
		}
	      sys_error (_("%s: %s: bad interpreter"), command, interp ? interp : "");
	      FREE (interp);
	      return (EX_NOEXEC);
	    }
#endif
	  errno = i;
	  file_error (command);
	}
      return ((i == ENOENT) ? EX_NOTFOUND : EX_NOEXEC);	/* XXX Posix.2 says that exit status is 126 */
    }

  /* This file is executable.
     If it begins with #!, then help out people with losing operating
     systems.  Otherwise, check to see if it is a binary file by seeing
     if the contents of the first line (or up to 80 characters) are in the
     ASCII set.  If it's a text file, execute the contents as shell commands,
     otherwise return 126 (EX_BINARY_FILE). */
  READ_SAMPLE_BUF (command, sample, sample_len);

  if (sample_len == 0)
    return (EXECUTION_SUCCESS);

  /* Is this supposed to be an executable script?
     If so, the format of the line is "#! interpreter [argument]".
     A single argument is allowed.  The BSD kernel restricts
     the length of the entire line to 32 characters (32 bytes
     being the size of the BSD exec header), but we allow 80
     characters. */
  if (sample_len > 0)
    {
#if !defined (HAVE_HASH_BANG_EXEC)
      if (sample_len > 2 && sample[0] == '#' && sample[1] == '!')
	return (execute_shell_script (sample, sample_len, command, args, env));
      else
#endif
      if (check_binary_file (sample, sample_len))
	{
	  internal_error (_("%s: cannot execute binary file: %s"), command, strerror (i));
	  return (EX_BINARY_FILE);
	}
    }

  /* We have committed to attempting to execute the contents of this file
     as shell commands. */

  initialize_subshell ();

  set_sigint_handler ();

  /* Insert the name of this shell into the argument list. */
  larray = strvec_len (args) + 1;
  args = strvec_resize (args, larray + 1);

  for (i = larray - 1; i; i--)
    args[i] = args[i - 1];

  args[0] = shell_name;
  args[1] = command;
  args[larray] = (char *)NULL;

  if (args[0][0] == '-')
    args[0]++;

#if defined (RESTRICTED_SHELL)
  if (restricted)
    change_flag ('r', FLAG_OFF);
#endif

  if (subshell_argv)
    {
      /* Can't free subshell_argv[0]; that is shell_name. */
      for (i = 1; i < subshell_argc; i++)
	free (subshell_argv[i]);
      free (subshell_argv);
    }

  dispose_command (currently_executing_command);	/* XXX */
  currently_executing_command = (COMMAND *)NULL;

  subshell_argc = larray;
  subshell_argv = args;
  subshell_envp = env;

  unbind_args ();	/* remove the positional parameters */

  longjmp (subshell_top_level, 1);
  /*NOTREACHED*/
}