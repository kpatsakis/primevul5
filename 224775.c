sv_funcnest (name)
     char *name;
{
  SHELL_VAR *v;
  intmax_t num;

  v = find_variable (name);
  if (v == 0)
    funcnest_max = 0;
  else if (legal_number (value_cell (v), &num) == 0)
    funcnest_max = 0;
  else
    funcnest_max = num;
}