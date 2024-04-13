initialize_shell_variables (env, privmode)
     char **env;
     int privmode;
{
  char *name, *string, *temp_string;
  int c, char_index, string_index, string_length, ro;
  SHELL_VAR *temp_var;

  create_variable_tables ();

  for (string_index = 0; string = env[string_index++]; )
    {
      char_index = 0;
      name = string;
      while ((c = *string++) && c != '=')
	;
      if (string[-1] == '=')
	char_index = string - name - 1;

      /* If there are weird things in the environment, like `=xxx' or a
	 string without an `=', just skip them. */
      if (char_index == 0)
	continue;

      /* ASSERT(name[char_index] == '=') */
      name[char_index] = '\0';
      /* Now, name = env variable name, string = env variable value, and
	 char_index == strlen (name) */

      temp_var = (SHELL_VAR *)NULL;

      /* If exported function, define it now.  Don't import functions from
	 the environment in privileged mode. */
      if (privmode == 0 && read_but_dont_execute == 0 && STREQN ("() {", string, 4))
	{
	  string_length = strlen (string);
	  temp_string = (char *)xmalloc (3 + string_length + char_index);

	  strcpy (temp_string, name);
	  temp_string[char_index] = ' ';
	  strcpy (temp_string + char_index + 1, string);

	  parse_and_execute (temp_string, name, SEVAL_NONINT|SEVAL_NOHIST);

	  /* Ancient backwards compatibility.  Old versions of bash exported
	     functions like name()=() {...} */
	  if (name[char_index - 1] == ')' && name[char_index - 2] == '(')
	    name[char_index - 2] = '\0';

	  if (temp_var = find_function (name))
	    {
	      VSETATTR (temp_var, (att_exported|att_imported));
	      array_needs_making = 1;
	    }
	  else
	    {
	      last_command_exit_value = 1;
	      report_error (_("error importing function definition for `%s'"), name);
	    }

	  /* ( */
	  if (name[char_index - 1] == ')' && name[char_index - 2] == '\0')
	    name[char_index - 2] = '(';		/* ) */
	}
#if defined (ARRAY_VARS)
#  if 0
      /* Array variables may not yet be exported. */
      else if (*string == '(' && string[1] == '[' && string[strlen (string) - 1] == ')')
	{
	  string_length = 1;
	  temp_string = extract_array_assignment_list (string, &string_length);
	  temp_var = assign_array_from_string (name, temp_string);
	  FREE (temp_string);
	  VSETATTR (temp_var, (att_exported | att_imported));
	  array_needs_making = 1;
	}
#  endif
#endif
#if 0
      else if (legal_identifier (name))
#else
      else
#endif
	{
	  ro = 0;
	  if (posixly_correct && STREQ (name, "SHELLOPTS"))
	    {
	      temp_var = find_variable ("SHELLOPTS");
	      ro = temp_var && readonly_p (temp_var);
	      if (temp_var)
		VUNSETATTR (temp_var, att_readonly);
	    }
	  temp_var = bind_variable (name, string, 0);
	  if (temp_var)
	    {
	      if (legal_identifier (name))
		VSETATTR (temp_var, (att_exported | att_imported));
	      else
		VSETATTR (temp_var, (att_exported | att_imported | att_invisible));
	      if (ro)
		VSETATTR (temp_var, att_readonly);
	      array_needs_making = 1;
	    }
	}

      name[char_index] = '=';
      /* temp_var can be NULL if it was an exported function with a syntax
	 error (a different bug, but it still shouldn't dump core). */
      if (temp_var && function_p (temp_var) == 0)	/* XXX not yet */
	{
	  CACHE_IMPORTSTR (temp_var, name);
	}
    }

  set_pwd ();

  /* Set up initial value of $_ */
  temp_var = set_if_not ("_", dollar_vars[0]);

  /* Remember this pid. */
  dollar_dollar_pid = getpid ();

  /* Now make our own defaults in case the vars that we think are
     important are missing. */
  temp_var = set_if_not ("PATH", DEFAULT_PATH_VALUE);
#if 0
  set_auto_export (temp_var);	/* XXX */
#endif

  temp_var = set_if_not ("TERM", "dumb");
#if 0
  set_auto_export (temp_var);	/* XXX */
#endif

#if defined (__QNX__)
  /* set node id -- don't import it from the environment */
  {
    char node_name[22];
#  if defined (__QNXNTO__)
    netmgr_ndtostr(ND2S_LOCAL_STR, ND_LOCAL_NODE, node_name, sizeof(node_name));
#  else
    qnx_nidtostr (getnid (), node_name, sizeof (node_name));
#  endif
    temp_var = bind_variable ("NODE", node_name, 0);
    set_auto_export (temp_var);
  }
#endif

  /* set up the prompts. */
  if (interactive_shell)
    {
#if defined (PROMPT_STRING_DECODE)
      set_if_not ("PS1", primary_prompt);
#else
      if (current_user.uid == -1)
	get_current_user_info ();
      set_if_not ("PS1", current_user.euid == 0 ? "# " : primary_prompt);
#endif
      set_if_not ("PS2", secondary_prompt);
    }
  set_if_not ("PS4", "+ ");

  /* Don't allow IFS to be imported from the environment. */
  temp_var = bind_variable ("IFS", " \t\n", 0);
  setifs (temp_var);

  /* Magic machine types.  Pretty convenient. */
  set_machine_vars ();

  /* Default MAILCHECK for interactive shells.  Defer the creation of a
     default MAILPATH until the startup files are read, because MAIL
     names a mail file if MAILPATH is not set, and we should provide a
     default only if neither is set. */
  if (interactive_shell)
    {
      temp_var = set_if_not ("MAILCHECK", posixly_correct ? "600" : "60");
      VSETATTR (temp_var, att_integer);
    }

  /* Do some things with shell level. */
  initialize_shell_level ();

  set_ppid ();

  /* Initialize the `getopts' stuff. */
  temp_var = bind_variable ("OPTIND", "1", 0);
  VSETATTR (temp_var, att_integer);
  getopts_reset (0);
  bind_variable ("OPTERR", "1", 0);
  sh_opterr = 1;

  if (login_shell == 1 && posixly_correct == 0)
    set_home_var ();

  /* Get the full pathname to THIS shell, and set the BASH variable
     to it. */
  name = get_bash_name ();
  temp_var = bind_variable ("BASH", name, 0);
  free (name);

  /* Make the exported environment variable SHELL be the user's login
     shell.  Note that the `tset' command looks at this variable
     to determine what style of commands to output; if it ends in "csh",
     then C-shell commands are output, else Bourne shell commands. */
  set_shell_var ();

  /* Make a variable called BASH_VERSION which contains the version info. */
  bind_variable ("BASH_VERSION", shell_version_string (), 0);
#if defined (ARRAY_VARS)
  make_vers_array ();
#endif

  if (command_execution_string)
    bind_variable ("BASH_EXECUTION_STRING", command_execution_string, 0);

  /* Find out if we're supposed to be in Posix.2 mode via an
     environment variable. */
  temp_var = find_variable ("POSIXLY_CORRECT");
  if (!temp_var)
    temp_var = find_variable ("POSIX_PEDANTIC");
  if (temp_var && imported_p (temp_var))
    sv_strict_posix (temp_var->name);

#if defined (HISTORY)
  /* Set history variables to defaults, and then do whatever we would
     do if the variable had just been set.  Do this only in the case
     that we are remembering commands on the history list. */
  if (remember_on_history)
    {
      name = bash_tilde_expand (posixly_correct ? "~/.sh_history" : "~/.bash_history", 0);

      set_if_not ("HISTFILE", name);
      free (name);
    }
#endif /* HISTORY */

  /* Seed the random number generator. */
  seedrand ();

  /* Handle some "special" variables that we may have inherited from a
     parent shell. */
  if (interactive_shell)
    {
      temp_var = find_variable ("IGNOREEOF");
      if (!temp_var)
	temp_var = find_variable ("ignoreeof");
      if (temp_var && imported_p (temp_var))
	sv_ignoreeof (temp_var->name);
    }

#if defined (HISTORY)
  if (interactive_shell && remember_on_history)
    {
      sv_history_control ("HISTCONTROL");
      sv_histignore ("HISTIGNORE");
      sv_histtimefmt ("HISTTIMEFORMAT");
    }
#endif /* HISTORY */

#if defined (READLINE) && defined (STRICT_POSIX)
  /* POSIXLY_CORRECT will only be 1 here if the shell was compiled
     -DSTRICT_POSIX */
  if (interactive_shell && posixly_correct && no_line_editing == 0)
    rl_prefer_env_winsize = 1;
#endif /* READLINE && STRICT_POSIX */

     /*
      * 24 October 2001
      *
      * I'm tired of the arguing and bug reports.  Bash now leaves SSH_CLIENT
      * and SSH2_CLIENT alone.  I'm going to rely on the shell_level check in
      * isnetconn() to avoid running the startup files more often than wanted.
      * That will, of course, only work if the user's login shell is bash, so
      * I've made that behavior conditional on SSH_SOURCE_BASHRC being defined
      * in config-top.h.
      */
#if 0
  temp_var = find_variable ("SSH_CLIENT");
  if (temp_var && imported_p (temp_var))
    {
      VUNSETATTR (temp_var, att_exported);
      array_needs_making = 1;
    }
  temp_var = find_variable ("SSH2_CLIENT");
  if (temp_var && imported_p (temp_var))
    {
      VUNSETATTR (temp_var, att_exported);
      array_needs_making = 1;
    }
#endif

  /* Get the user's real and effective user ids. */
  uidset ();

  temp_var = find_variable ("BASH_XTRACEFD");
  if (temp_var && imported_p (temp_var))
    sv_xtracefd (temp_var->name);

  /* Initialize the dynamic variables, and seed their values. */
  initialize_dynamic_variables ();
}