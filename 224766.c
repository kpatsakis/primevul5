execute_case_command (case_command)
     CASE_COM *case_command;
{
  register WORD_LIST *list;
  WORD_LIST *wlist, *es;
  PATTERN_LIST *clauses;
  char *word, *pattern;
  int retval, match, ignore_return, save_line_number;

  save_line_number = line_number;
  line_number = case_command->line;

  command_string_index = 0;
  print_case_command_head (case_command);

  if (echo_command_at_execute)
    xtrace_print_case_command_head (case_command);

#if 0
  if (signal_in_progress (DEBUG_TRAP) == 0 && (this_command_name == 0 || (STREQ (this_command_name, "trap") == 0)))
#else
  if (signal_in_progress (DEBUG_TRAP) == 0 && running_trap == 0)
#endif
    {
      FREE (the_printed_command_except_trap);
      the_printed_command_except_trap = savestring (the_printed_command);
    }

  retval = run_debug_trap();
#if defined (DEBUGGER)
  /* In debugging mode, if the DEBUG trap returns a non-zero status, we
     skip the command. */
  if (debugging_mode && retval != EXECUTION_SUCCESS)
    {
      line_number = save_line_number;
      return (EXECUTION_SUCCESS);
    }
#endif

  wlist = expand_word_unsplit (case_command->word, 0);
  word = wlist ? string_list (wlist) : savestring ("");
  dispose_words (wlist);

  retval = EXECUTION_SUCCESS;
  ignore_return = case_command->flags & CMD_IGNORE_RETURN;

  begin_unwind_frame ("case");
  add_unwind_protect ((Function *)xfree, word);

#define EXIT_CASE()  goto exit_case_command

  for (clauses = case_command->clauses; clauses; clauses = clauses->next)
    {
      QUIT;
      for (list = clauses->patterns; list; list = list->next)
	{
	  es = expand_word_leave_quoted (list->word, 0);

	  if (es && es->word && es->word->word && *(es->word->word))
	    pattern = quote_string_for_globbing (es->word->word, QGLOB_CVTNULL);
	  else
	    {
	      pattern = (char *)xmalloc (1);
	      pattern[0] = '\0';
	    }

	  /* Since the pattern does not undergo quote removal (as per
	     Posix.2, section 3.9.4.3), the strmatch () call must be able
	     to recognize backslashes as escape characters. */
	  match = strmatch (pattern, word, FNMATCH_EXTFLAG|FNMATCH_IGNCASE) != FNM_NOMATCH;
	  free (pattern);

	  dispose_words (es);

	  if (match)
	    {
	      do
		{
		  if (clauses->action && ignore_return)
		    clauses->action->flags |= CMD_IGNORE_RETURN;
		  retval = execute_command (clauses->action);
		}
	      while ((clauses->flags & CASEPAT_FALLTHROUGH) && (clauses = clauses->next));
	      if (clauses == 0 || (clauses->flags & CASEPAT_TESTNEXT) == 0)
		EXIT_CASE ();
	      else
		break;
	    }

	  QUIT;
	}
    }

exit_case_command:
  free (word);
  discard_unwind_frame ("case");
  line_number = save_line_number;
  return (retval);
}