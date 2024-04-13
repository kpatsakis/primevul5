execute_select_command (select_command)
     SELECT_COM *select_command;
{
  WORD_LIST *releaser, *list;
  SHELL_VAR *v;
  char *identifier, *ps3_prompt, *selection;
  int retval, list_len, show_menu, save_line_number;

  if (check_identifier (select_command->name, 1) == 0)
    return (EXECUTION_FAILURE);

  save_line_number = line_number;
  line_number = select_command->line;

  command_string_index = 0;
  print_select_command_head (select_command);

  if (echo_command_at_execute)
    xtrace_print_select_command_head (select_command);

#if 0
  if (signal_in_progress (DEBUG_TRAP) == 0 && (this_command_name == 0 || (STREQ (this_command_name, "trap") == 0)))
#else
  if (signal_in_progress (DEBUG_TRAP) == 0 && running_trap == 0)
#endif
    {
      FREE (the_printed_command_except_trap);
      the_printed_command_except_trap = savestring (the_printed_command);
    }

  retval = run_debug_trap ();
#if defined (DEBUGGER)
  /* In debugging mode, if the DEBUG trap returns a non-zero status, we
     skip the command. */
  if (debugging_mode && retval != EXECUTION_SUCCESS)
    return (EXECUTION_SUCCESS);
#endif

  loop_level++;
  identifier = select_command->name->word;

  /* command and arithmetic substitution, parameter and variable expansion,
     word splitting, pathname expansion, and quote removal. */
  list = releaser = expand_words_no_vars (select_command->map_list);
  list_len = list_length (list);
  if (list == 0 || list_len == 0)
    {
      if (list)
	dispose_words (list);
      line_number = save_line_number;
      return (EXECUTION_SUCCESS);
    }

  begin_unwind_frame ("select");
  add_unwind_protect (dispose_words, releaser);

  if (select_command->flags & CMD_IGNORE_RETURN)
    select_command->action->flags |= CMD_IGNORE_RETURN;

  retval = EXECUTION_SUCCESS;
  show_menu = 1;

  while (1)
    {
      line_number = select_command->line;
      ps3_prompt = get_string_value ("PS3");
      if (ps3_prompt == 0)
	ps3_prompt = "#? ";

      QUIT;
      selection = select_query (list, list_len, ps3_prompt, show_menu);
      QUIT;
      if (selection == 0)
	{
	  /* select_query returns EXECUTION_FAILURE if the read builtin
	     fails, so we want to return failure in this case. */
	  retval = EXECUTION_FAILURE;
	  break;
	}

      v = bind_variable (identifier, selection, 0);
      if (readonly_p (v) || noassign_p (v))
	{
	  if (readonly_p (v) && interactive_shell == 0 && posixly_correct)
	    {
	      last_command_exit_value = EXECUTION_FAILURE;
	      jump_to_top_level (FORCE_EOF);
	    }
	  else
	    {
	      dispose_words (releaser);
	      discard_unwind_frame ("select");
	      loop_level--;
	      line_number = save_line_number;
	      return (EXECUTION_FAILURE);
	    }
	}

      retval = execute_command (select_command->action);

      REAP ();
      QUIT;

      if (breaking)
	{
	  breaking--;
	  break;
	}

      if (continuing)
	{
	  continuing--;
	  if (continuing)
	    break;
	}

#if defined (KSH_COMPATIBLE_SELECT)
      show_menu = 0;
      selection = get_string_value ("REPLY");
      if (selection && *selection == '\0')
        show_menu = 1;
#endif
    }

  loop_level--;
  line_number = save_line_number;

  dispose_words (releaser);
  discard_unwind_frame ("select");
  return (retval);
}