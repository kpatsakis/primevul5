execute_shell_script (sample, sample_len, command, args, env)
     char *sample;
     int sample_len;
     char *command;
     char **args, **env;
{
  char *execname, *firstarg;
  int i, start, size_increment, larry;

  /* Find the name of the interpreter to exec. */
  execname = getinterp (sample, sample_len, &i);
  size_increment = 1;

  /* Now the argument, if any. */
  for (firstarg = (char *)NULL, start = i; WHITECHAR(i); i++)
    ;

  /* If there is more text on the line, then it is an argument for the
     interpreter. */

  if (STRINGCHAR(i))  
    {
      for (start = i; STRINGCHAR(i); i++)
	;
      firstarg = substring ((char *)sample, start, i);
      size_increment = 2;
    }

  larry = strvec_len (args) + size_increment;
  args = strvec_resize (args, larry + 1);

  for (i = larry - 1; i; i--)
    args[i] = args[i - size_increment];

  args[0] = execname;
  if (firstarg)
    {
      args[1] = firstarg;
      args[2] = command;
    }
  else
    args[1] = command;

  args[larry] = (char *)NULL;

  return (shell_execve (execname, args, env));
}