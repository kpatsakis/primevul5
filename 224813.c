get_comp_wordbreaks (var)
     SHELL_VAR *var;
{
  /* If we don't have anything yet, assign a default value. */
  if (rl_completer_word_break_characters == 0 && bash_readline_initialized == 0)
    enable_hostname_completion (perform_hostname_completion);

  FREE (value_cell (var));
  var_setvalue (var, savestring (rl_completer_word_break_characters));

  return (var);
}