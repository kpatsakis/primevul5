make_variable_value (var, value, flags)
     SHELL_VAR *var;
     char *value;
     int flags;
{
  char *retval, *oval;
  intmax_t lval, rval;
  int expok, olen, op;

  /* If this variable has had its type set to integer (via `declare -i'),
     then do expression evaluation on it and store the result.  The
     functions in expr.c (evalexp()) and bind_int_variable() are responsible
     for turning off the integer flag if they don't want further
     evaluation done. */
  if (integer_p (var))
    {
      if (flags & ASS_APPEND)
	{
	  oval = value_cell (var);
	  lval = evalexp (oval, &expok);	/* ksh93 seems to do this */
	  if (expok == 0)
	    {
	      top_level_cleanup ();
	      jump_to_top_level (DISCARD);
	    }
	}
      rval = evalexp (value, &expok);
      if (expok == 0)
	{
	  top_level_cleanup ();
	  jump_to_top_level (DISCARD);
	}
      if (flags & ASS_APPEND)
	rval += lval;
      retval = itos (rval);
    }
#if defined (CASEMOD_ATTRS)
  else if (capcase_p (var) || uppercase_p (var) || lowercase_p (var))
    {
      if (flags & ASS_APPEND)
	{
	  oval = get_variable_value (var);
	  if (oval == 0)	/* paranoia */
	    oval = "";
	  olen = STRLEN (oval);
	  retval = (char *)xmalloc (olen + (value ? STRLEN (value) : 0) + 1);
	  strcpy (retval, oval);
	  if (value)
	    strcpy (retval+olen, value);
	}
      else if (*value)
	retval = savestring (value);
      else
	{
	  retval = (char *)xmalloc (1);
	  retval[0] = '\0';
	}
      op = capcase_p (var) ? CASE_CAPITALIZE
			 : (uppercase_p (var) ? CASE_UPPER : CASE_LOWER);
      oval = sh_modcase (retval, (char *)0, op);
      free (retval);
      retval = oval;
    }
#endif /* CASEMOD_ATTRS */
  else if (value)
    {
      if (flags & ASS_APPEND)
	{
	  oval = get_variable_value (var);
	  if (oval == 0)	/* paranoia */
	    oval = "";
	  olen = STRLEN (oval);
	  retval = (char *)xmalloc (olen + (value ? STRLEN (value) : 0) + 1);
	  strcpy (retval, oval);
	  if (value)
	    strcpy (retval+olen, value);
	}
      else if (*value)
	retval = savestring (value);
      else
	{
	  retval = (char *)xmalloc (1);
	  retval[0] = '\0';
	}
    }
  else
    retval = (char *)NULL;

  return retval;
}