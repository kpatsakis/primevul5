Item_bool_rowready_func2* Le_creator::create(THD *thd, Item *a, Item *b) const
{
  return new(thd->mem_root) Item_func_le(thd, a, b);
}