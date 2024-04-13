bool Item_func_between::count_sargable_conds(void *arg)
{
  SELECT_LEX *sel= (SELECT_LEX *) arg;
  sel->cond_count++;
  sel->between_count++;
  return 0;
}