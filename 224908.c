assign_in_env (word, flags)
     WORD_DESC *word;
     int flags;
{
  int offset;
  char *name, *temp, *value;
  SHELL_VAR *var;
  const char *string;

  string = word->word;

  offset = assignment (string, 0);
  name = savestring (string);
  value = (char *)NULL;

  if (name[offset] == '=')
    {
      name[offset] = 0;

      /* ignore the `+' when assigning temporary environment */
      if (name[offset - 1] == '+')
	name[offset - 1] = '\0';

      var = find_variable (name);
      if (var && (readonly_p (var) || noassign_p (var)))
	{
	  if (readonly_p (var))
	    err_readonly (name);
	  free (name);
  	  return (0);
	}

      temp = name + offset + 1;
      value = expand_assignment_string_to_string (temp, 0);
    }

  if (temporary_env == 0)
    temporary_env = hash_create (TEMPENV_HASH_BUCKETS);

  var = hash_lookup (name, temporary_env);
  if (var == 0)
    var = make_new_variable (name, temporary_env);
  else
    FREE (value_cell (var));

  if (value == 0)
    {
      value = (char *)xmalloc (1);	/* like do_assignment_internal */
      value[0] = '\0';
    }

  var_setvalue (var, value);
  var->attributes |= (att_exported|att_tempvar);
  var->context = variable_context;	/* XXX */

  INVALIDATE_EXPORTSTR (var);
  var->exportstr = mk_env_string (name, value);

  array_needs_making = 1;

#if 0
  if (ifsname (name))
    setifs (var);
else
#endif
  if (flags)
    stupidly_hack_special_variables (name);

  if (echo_command_at_execute)
    /* The Korn shell prints the `+ ' in front of assignment statements,
	so we do too. */
    xtrace_print_assignment (name, value, 0, 1);

  free (name);
  return 1;
}