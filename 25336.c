Item_bool_rowready_func2* Gt_creator::create(THD *thd, Item *a, Item *b) const
{
  return new(thd->mem_root) Item_func_gt(thd, a, b);
}