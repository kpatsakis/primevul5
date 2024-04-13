print_var_value (var, quote)
     SHELL_VAR *var;
     int quote;
{
  char *t;

  if (var_isset (var) == 0)
    return;

  if (quote && posixly_correct == 0 && ansic_shouldquote (value_cell (var)))
    {
      t = ansic_quote (value_cell (var), 0, (int *)0);
      printf ("%s", t);
      free (t);
    }
  else if (quote && sh_contains_shell_metas (value_cell (var)))
    {
      t = sh_single_quote (value_cell (var));
      printf ("%s", t);
      free (t);
    }
  else
    printf ("%s", value_cell (var));
}