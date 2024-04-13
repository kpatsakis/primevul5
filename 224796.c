get_bash_name ()
{
  char *name;

  if ((login_shell == 1) && RELPATH(shell_name))
    {
      if (current_user.shell == 0)
	get_current_user_info ();
      name = savestring (current_user.shell);
    }
  else if (ABSPATH(shell_name))
    name = savestring (shell_name);
  else if (shell_name[0] == '.' && shell_name[1] == '/')
    {
      /* Fast path for common case. */
      char *cdir;
      int len;

      cdir = get_string_value ("PWD");
      if (cdir)
	{
	  len = strlen (cdir);
	  name = (char *)xmalloc (len + strlen (shell_name) + 1);
	  strcpy (name, cdir);
	  strcpy (name + len, shell_name + 1);
	}
      else
	name = savestring (shell_name);
    }
  else
    {
      char *tname;
      int s;

      tname = find_user_command (shell_name);

      if (tname == 0)
	{
	  /* Try the current directory.  If there is not an executable
	     there, just punt and use the login shell. */
	  s = file_status (shell_name);
	  if (s & FS_EXECABLE)
	    {
	      tname = make_absolute (shell_name, get_string_value ("PWD"));
	      if (*shell_name == '.')
		{
		  name = sh_canonpath (tname, PATH_CHECKDOTDOT|PATH_CHECKEXISTS);
		  if (name == 0)
		    name = tname;
		  else
		    free (tname);
		}
	     else
		name = tname;
	    }
	  else
	    {
	      if (current_user.shell == 0)
		get_current_user_info ();
	      name = savestring (current_user.shell);
	    }
	}
      else
	{
	  name = full_pathname (tname);
	  free (tname);
	}
    }

  return (name);
}