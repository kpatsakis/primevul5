executing_line_number ()
{
  if (executing && showing_function_line == 0 &&
      (variable_context == 0 || interactive_shell == 0) &&
      currently_executing_command)
    {
#if defined (COND_COMMAND)
      if (currently_executing_command->type == cm_cond)
	return currently_executing_command->value.Cond->line;
#endif
#if defined (DPAREN_ARITHMETIC)
      else if (currently_executing_command->type == cm_arith)
	return currently_executing_command->value.Arith->line;
#endif
#if defined (ARITH_FOR_COMMAND)
      else if (currently_executing_command->type == cm_arith_for)
	return currently_executing_command->value.ArithFor->line;
#endif

	return line_number;
    }
  else
    return line_number;
}