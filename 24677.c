bool Item_func_null_predicate::count_sargable_conds(void *arg)
{
  ((SELECT_LEX*) arg)->cond_count++;
  return 0;
}