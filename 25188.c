Item *Item_cond_and::copy_andor_structure(THD *thd)
{
  Item_cond_and *item;
  if ((item= new (thd->mem_root) Item_cond_and(thd, this)))
    item->copy_andor_arguments(thd, this);
  return item;
}