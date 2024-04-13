pop_dollar_vars ()
{
  if (!dollar_arg_stack || dollar_arg_stack_index == 0)
    return;

  remember_args (dollar_arg_stack[--dollar_arg_stack_index], 1);
  dispose_words (dollar_arg_stack[dollar_arg_stack_index]);
  dollar_arg_stack[dollar_arg_stack_index] = (WORD_LIST *)NULL;
  set_dollar_vars_unchanged ();
}