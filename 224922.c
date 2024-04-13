execute_cond_node (cond)
     COND_COM *cond;
{
  int result, invert, patmatch, rmatch, mflags, ignore;
  char *arg1, *arg2;

  invert = (cond->flags & CMD_INVERT_RETURN);
  ignore = (cond->flags & CMD_IGNORE_RETURN);
  if (ignore)
    {
      if (cond->left)
	cond->left->flags |= CMD_IGNORE_RETURN;
      if (cond->right)
	cond->right->flags |= CMD_IGNORE_RETURN;
    }
      
  if (cond->type == COND_EXPR)
    result = execute_cond_node (cond->left);
  else if (cond->type == COND_OR)
    {
      result = execute_cond_node (cond->left);
      if (result != EXECUTION_SUCCESS)
	result = execute_cond_node (cond->right);
    }
  else if (cond->type == COND_AND)
    {
      result = execute_cond_node (cond->left);
      if (result == EXECUTION_SUCCESS)
	result = execute_cond_node (cond->right);
    }
  else if (cond->type == COND_UNARY)
    {
      if (ignore)
	comsub_ignore_return++;
      arg1 = cond_expand_word (cond->left->op, 0);
      if (ignore)
	comsub_ignore_return--;
      if (arg1 == 0)
	arg1 = nullstr;
      if (echo_command_at_execute)
	xtrace_print_cond_term (cond->type, invert, cond->op, arg1, (char *)NULL);
      result = unary_test (cond->op->word, arg1) ? EXECUTION_SUCCESS : EXECUTION_FAILURE;
      if (arg1 != nullstr)
	free (arg1);
    }
  else if (cond->type == COND_BINARY)
    {
      rmatch = 0;
      patmatch = (((cond->op->word[1] == '=') && (cond->op->word[2] == '\0') &&
		   (cond->op->word[0] == '!' || cond->op->word[0] == '=')) ||
		  (cond->op->word[0] == '=' && cond->op->word[1] == '\0'));
#if defined (COND_REGEXP)
      rmatch = (cond->op->word[0] == '=' && cond->op->word[1] == '~' &&
		cond->op->word[2] == '\0');
#endif

      if (ignore)
	comsub_ignore_return++;
      arg1 = cond_expand_word (cond->left->op, 0);
      if (ignore)
	comsub_ignore_return--;
      if (arg1 == 0)
	arg1 = nullstr;
      if (ignore)
	comsub_ignore_return++;
      arg2 = cond_expand_word (cond->right->op,
			       (rmatch && shell_compatibility_level > 31) ? 2 : (patmatch ? 1 : 0));
      if (ignore)
	comsub_ignore_return--;
      if (arg2 == 0)
	arg2 = nullstr;

      if (echo_command_at_execute)
	xtrace_print_cond_term (cond->type, invert, cond->op, arg1, arg2);

#if defined (COND_REGEXP)
      if (rmatch)
	{
	  mflags = SHMAT_PWARN;
#if defined (ARRAY_VARS)
	  mflags |= SHMAT_SUBEXP;
#endif

	  result = sh_regmatch (arg1, arg2, mflags);
	}
      else
#endif /* COND_REGEXP */
	{
	  int oe;
	  oe = extended_glob;
	  extended_glob = 1;
	  result = binary_test (cond->op->word, arg1, arg2, TEST_PATMATCH|TEST_ARITHEXP|TEST_LOCALE)
				  ? EXECUTION_SUCCESS
				  : EXECUTION_FAILURE;
	  extended_glob = oe;
	}
      if (arg1 != nullstr)
	free (arg1);
      if (arg2 != nullstr)
	free (arg2);
    }
  else
    {
      command_error ("execute_cond_node", CMDERR_BADTYPE, cond->type, 0);
      jump_to_top_level (DISCARD);
      result = EXECUTION_FAILURE;
    }

  if (invert)
    result = (result == EXECUTION_SUCCESS) ? EXECUTION_FAILURE : EXECUTION_SUCCESS;

  return result;
}