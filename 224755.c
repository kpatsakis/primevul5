execute_for_command (for_command)
     FOR_COM *for_command;
{
  register WORD_LIST *releaser, *list;
  SHELL_VAR *v;
  char *identifier;
  int retval, save_line_number;
#if 0
  SHELL_VAR *old_value = (SHELL_VAR *)NULL; /* Remember the old value of x. */
#endif

  save_line_number = line_number;
  if (check_identifier (for_command->name, 1) == 0)
    {
      if (posixly_correct && interactive_shell == 0)
	{
	  last_command_exit_value = EX_BADUSAGE;
	  jump_to_top_level (ERREXIT);
	}
      return (EXECUTION_FAILURE);
    }

  loop_level++;
  identifier = for_command->name->word;

  list = releaser = expand_words_no_vars (for_command->map_list);

  begin_unwind_frame ("for");
  add_unwind_protect (dispose_words, releaser);

#if 0
  if (lexical_scoping)
    {
      old_value = copy_variable (find_variable (identifier));
      if (old_value)
	add_unwind_protect (dispose_variable, old_value);
    }
#endif

  if (for_command->flags & CMD_IGNORE_RETURN)
    for_command->action->flags |= CMD_IGNORE_RETURN;

  for (retval = EXECUTION_SUCCESS; list; list = list->next)
    {
      QUIT;

      line_number = for_command->line;

      /* Remember what this command looks like, for debugger. */
      command_string_index = 0;
      print_for_command_head (for_command);

      if (echo_command_at_execute)
	xtrace_print_for_command_head (for_command);

      /* Save this command unless it's a trap command and we're not running
	 a debug trap. */
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
        continue;
#endif

      this_command_name = (char *)NULL;
      v = bind_variable (identifier, list->word->word, 0);
      if (readonly_p (v) || noassign_p (v))
	{
	  line_number = save_line_number;
	  if (readonly_p (v) && interactive_shell == 0 && posixly_correct)
	    {
	      last_command_exit_value = EXECUTION_FAILURE;
	      jump_to_top_level (FORCE_EOF);
	    }
	  else
	    {
	      dispose_words (releaser);
	      discard_unwind_frame ("for");
	      loop_level--;
	      return (EXECUTION_FAILURE);
	    }
	}
      retval = execute_command (for_command->action);
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
    }

  loop_level--;
  line_number = save_line_number;

#if 0
  if (lexical_scoping)
    {
      if (!old_value)
        unbind_variable (identifier);
      else
	{
	  SHELL_VAR *new_value;

	  new_value = bind_variable (identifier, value_cell(old_value), 0);
	  new_value->attributes = old_value->attributes;
	  dispose_variable (old_value);
	}
    }
#endif

  dispose_words (releaser);
  discard_unwind_frame ("for");
  return (retval);
}