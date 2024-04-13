dispose_saved_dollar_vars ()
{
  if (!dollar_arg_stack || dollar_arg_stack_index == 0)
    return;

  dispose_words (dollar_arg_stack[dollar_arg_stack_index]);
  dollar_arg_stack[dollar_arg_stack_index] = (WORD_LIST *)NULL;
}