Item_bool_rowready_func2* Le_creator::create_swap(THD *thd, Item *a, Item *b) const
{
  return new(thd->mem_root) Item_func_ge(thd, b, a);
}