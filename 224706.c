get_groupset (self)
     SHELL_VAR *self;
{
  register int i;
  int ng;
  ARRAY *a;
  static char **group_set = (char **)NULL;

  if (group_set == 0)
    {
      group_set = get_group_list (&ng);
      a = array_cell (self);
      for (i = 0; i < ng; i++)
	array_insert (a, i, group_set[i]);
    }
  return (self);
}