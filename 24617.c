Item_bool_rowready_func2* Ne_creator::create(THD *thd, Item *a, Item *b) const
{
  return new(thd->mem_root) Item_func_ne(thd, a, b);
}