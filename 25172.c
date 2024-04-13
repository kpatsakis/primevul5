Item *Item_func_gt::negated_item(THD *thd) /* a > b  ->  a <= b */
{
  return new (thd->mem_root) Item_func_le(thd, args[0], args[1]);
}