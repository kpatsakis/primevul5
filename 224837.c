sv_ignoreeof (name)
     char *name;
{
  SHELL_VAR *tmp_var;
  char *temp;

  eof_encountered = 0;

  tmp_var = find_variable (name);
  ignoreeof = tmp_var != 0;
  temp = tmp_var ? value_cell (tmp_var) : (char *)NULL;
  if (temp)
    eof_encountered_limit = (*temp && all_digits (temp)) ? atoi (temp) : 10;
  set_shellopts ();	/* make sure `ignoreeof' is/is not in $SHELLOPTS */
}