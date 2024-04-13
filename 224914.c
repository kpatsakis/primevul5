bind_var_to_int (var, val)
     char *var;
     intmax_t val;
{
  char ibuf[INT_STRLEN_BOUND (intmax_t) + 1], *p;

  p = fmtulong (val, 10, ibuf, sizeof (ibuf), 0);
  return (bind_int_variable (var, p));
}