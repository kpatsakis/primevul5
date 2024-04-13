bind_int_variable (lhs, rhs)
     char *lhs, *rhs;
{
  register SHELL_VAR *v;
  int isint, isarr;

  isint = isarr = 0;
#if defined (ARRAY_VARS)
  if (valid_array_reference (lhs))
    {
      isarr = 1;
      v = array_variable_part (lhs, (char **)0, (int *)0);
    }
  else
#endif
    v = find_variable (lhs);

  if (v)
    {
      isint = integer_p (v);
      VUNSETATTR (v, att_integer);
    }

#if defined (ARRAY_VARS)
  if (isarr)
    v = assign_array_element (lhs, rhs, 0);
  else
#endif
    v = bind_variable (lhs, rhs, 0);

  if (v && isint)
    VSETATTR (v, att_integer);

  return (v);
}