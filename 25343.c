Item *Item_cond_or::copy_andor_structure(THD *thd)
{
  Item_cond_or *item;
  if ((item= new (thd->mem_root) Item_cond_or(thd, this)))
    item->copy_andor_arguments(thd, this);
  return item;
}