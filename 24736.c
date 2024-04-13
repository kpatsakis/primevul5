Item *Item_func_isnotnull::neg_transformer(THD *thd)
{
  Item *item= new (thd->mem_root) Item_func_isnull(thd, args[0]);
  return item;
}