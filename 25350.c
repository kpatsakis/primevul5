Item *Item_func_eq::negated_item(THD *thd) /* a = b  ->  a != b */
{
  return new (thd->mem_root) Item_func_ne(thd, args[0], args[1]);
}