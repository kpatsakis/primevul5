execute_if_command (if_command)
     IF_COM *if_command;
{
  int return_value, save_line_number;

  save_line_number = line_number;
  if_command->test->flags |= CMD_IGNORE_RETURN;
  return_value = execute_command (if_command->test);
  line_number = save_line_number;

  if (return_value == EXECUTION_SUCCESS)
    {
      QUIT;

      if (if_command->true_case && (if_command->flags & CMD_IGNORE_RETURN))
	if_command->true_case->flags |= CMD_IGNORE_RETURN;

      return (execute_command (if_command->true_case));
    }
  else
    {
      QUIT;

      if (if_command->false_case && (if_command->flags & CMD_IGNORE_RETURN))
	if_command->false_case->flags |= CMD_IGNORE_RETURN;

      return (execute_command (if_command->false_case));
    }
}