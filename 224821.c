stupidly_hack_special_variables (name)
     char *name;
{
  static int sv_sorted = 0;
  int i;

  if (sv_sorted == 0)	/* shouldn't need, but it's fairly cheap. */
    {
      qsort (special_vars, N_SPECIAL_VARS, sizeof (special_vars[0]),
		(QSFUNC *)sv_compare);
      sv_sorted = 1;
    }

  i = find_special_var (name);
  if (i != -1)
    (*(special_vars[i].function)) (name);
}