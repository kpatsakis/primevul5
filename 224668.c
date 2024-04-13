assign_comp_wordbreaks (self, value, unused, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t unused;
     char *key;
{
  if (rl_completer_word_break_characters &&
      rl_completer_word_break_characters != rl_basic_word_break_characters)
    free (rl_completer_word_break_characters);

  rl_completer_word_break_characters = savestring (value);
  return self;
}