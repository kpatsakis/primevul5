init_seconds_var ()
{
  SHELL_VAR *v;

  v = find_variable ("SECONDS");
  if (v)
    {
      if (legal_number (value_cell(v), &seconds_value_assigned) == 0)
	seconds_value_assigned = 0;
    }
  INIT_DYNAMIC_VAR ("SECONDS", (v ? value_cell (v) : (char *)NULL), get_seconds, assign_seconds);
  return v;      
}