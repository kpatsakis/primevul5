eval_arith_for_expr (l, okp)
     WORD_LIST *l;
     int *okp;
{
  WORD_LIST *new;
  intmax_t expresult;
  int r;

  new = expand_words_no_vars (l);
  if (new)
    {
      if (echo_command_at_execute)
	xtrace_print_arith_cmd (new);
      this_command_name = "((";		/* )) for expression error messages */

      command_string_index = 0;
      print_arith_command (new);
      if (signal_in_progress (DEBUG_TRAP) == 0)
	{
	  FREE (the_printed_command_except_trap);
	  the_printed_command_except_trap = savestring (the_printed_command);
	}

      r = run_debug_trap ();
      /* In debugging mode, if the DEBUG trap returns a non-zero status, we
	 skip the command. */
#if defined (DEBUGGER)
      if (debugging_mode == 0 || r == EXECUTION_SUCCESS)
	expresult = evalexp (new->word->word, okp);
      else
	{
	  expresult = 0;
	  if (okp)
	    *okp = 1;
	}
#else
      expresult = evalexp (new->word->word, okp);
#endif
      dispose_words (new);
    }
  else
    {
      expresult = 0;
      if (okp)
	*okp = 1;
    }
  return (expresult);
}