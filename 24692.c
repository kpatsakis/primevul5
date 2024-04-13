Item *Item_func_ge::negated_item(THD *thd) /* a >= b  ->  a < b */
{
  return new (thd->mem_root) Item_func_lt(thd, args[0], args[1]);
}