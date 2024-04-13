sv_winsize (name)
     char *name;
{
  SHELL_VAR *v;
  intmax_t xd;
  int d;

  if (posixly_correct == 0 || interactive_shell == 0 || no_line_editing)
    return;

  v = find_variable (name);
  if (v == 0 || var_isnull (v))
    rl_reset_screen_size ();
  else
    {
      if (legal_number (value_cell (v), &xd) == 0)
	return;
      winsize_assignment = 1;
      d = xd;			/* truncate */
      if (name[0] == 'L')	/* LINES */
	rl_set_screen_size (d, -1);
      else			/* COLUMNS */
	rl_set_screen_size (-1, d);
      winsize_assignment = 0;
    }
}