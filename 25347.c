bool Item_bool_func2::count_sargable_conds(void *arg)
{
  ((SELECT_LEX*) arg)->cond_count++;
  return 0;
}