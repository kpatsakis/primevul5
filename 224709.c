pop_args ()
{
#if defined (ARRAY_VARS) && defined (DEBUGGER)
  SHELL_VAR *bash_argv_v, *bash_argc_v;
  ARRAY *bash_argv_a, *bash_argc_a;
  ARRAY_ELEMENT *ce;
  intmax_t i;

  GET_ARRAY_FROM_VAR ("BASH_ARGV", bash_argv_v, bash_argv_a);
  GET_ARRAY_FROM_VAR ("BASH_ARGC", bash_argc_v, bash_argc_a);

  ce = array_shift (bash_argc_a, 1, 0);
  if (ce == 0 || legal_number (element_value (ce), &i) == 0)
    i = 0;

  for ( ; i > 0; i--)
    array_pop (bash_argv_a);
  array_dispose_element (ce);
#endif /* ARRAY_VARS && DEBUGGER */
}