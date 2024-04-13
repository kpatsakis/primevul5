Item_bool_rowready_func2* Ge_creator::create_swap(THD *thd, Item *a, Item *b) const
{
  return new(thd->mem_root) Item_func_le(thd, b, a);
}