make_env_array_from_var_list (vars)
     SHELL_VAR **vars;
{
  register int i, list_index;
  register SHELL_VAR *var;
  char **list, *value;

  list = strvec_create ((1 + strvec_len ((char **)vars)));

#define USE_EXPORTSTR (value == var->exportstr)

  for (i = 0, list_index = 0; var = vars[i]; i++)
    {
#if defined (__CYGWIN__)
      /* We don't use the exportstr stuff on Cygwin at all. */
      INVALIDATE_EXPORTSTR (var);
#endif
      if (var->exportstr)
	value = var->exportstr;
      else if (function_p (var))
	value = named_function_string ((char *)NULL, function_cell (var), 0);
#if defined (ARRAY_VARS)
      else if (array_p (var))
#  if 0
	value = array_to_assignment_string (array_cell (var));
#  else
	continue;	/* XXX array vars cannot yet be exported */
#  endif
      else if (assoc_p (var))
#  if 0
	value = assoc_to_assignment_string (assoc_cell (var));
#  else
	continue;	/* XXX associative array vars cannot yet be exported */
#  endif
#endif
      else
	value = value_cell (var);

      if (value)
	{
	  /* Gee, I'd like to get away with not using savestring() if we're
	     using the cached exportstr... */
	  list[list_index] = USE_EXPORTSTR ? savestring (value)
					   : mk_env_string (var->name, value);

	  if (USE_EXPORTSTR == 0)
	    SAVE_EXPORTSTR (var, list[list_index]);

	  list_index++;
#undef USE_EXPORTSTR

#if 0	/* not yet */
#if defined (ARRAY_VARS)
	  if (array_p (var) || assoc_p (var))
	    free (value);
#endif
#endif
	}
    }

  list[list_index] = (char *)NULL;
  return (list);
}