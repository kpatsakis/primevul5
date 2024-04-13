Item_cond::Item_cond(THD *thd, Item *i1, Item *i2):
  Item_bool_func(thd), abort_on_null(0)
{
  list.push_back(i1, thd->mem_root);
  list.push_back(i2, thd->mem_root);
}