push_dollar_vars ()
{
  if (dollar_arg_stack_index + 2 > dollar_arg_stack_slots)
    {
      dollar_arg_stack = (WORD_LIST **)
	xrealloc (dollar_arg_stack, (dollar_arg_stack_slots += 10)
		  * sizeof (WORD_LIST *));
    }
  dollar_arg_stack[dollar_arg_stack_index++] = list_rest_of_args ();
  dollar_arg_stack[dollar_arg_stack_index] = (WORD_LIST *)NULL;
}