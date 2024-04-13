Item_bool_rowready_func2* Eq_creator::create_swap(THD *thd, Item *a, Item *b) const
{
  return new(thd->mem_root) Item_func_eq(thd, b, a);
}