bool Item_equal::merge_with_check(THD *thd, Item_equal *item, bool save_merged)
{
  bool intersected= FALSE;
  Item_equal_fields_iterator_slow fi(*item);
  
  while (fi++)
  {
    if (contains(fi.get_curr_field()))
    {
      intersected= TRUE;
      if (!save_merged)
        fi.remove();
    }
  }
  if (intersected)
  {
    if (!save_merged)
      merge(thd, item);
    else
    {
      Item *c= item->get_const();
      if (c)
        add_const(thd, c);
      if (!cond_false)
      {
        Item *item;
        fi.rewind();
        while ((item= fi++))
	{
          if (!contains(fi.get_curr_field()))
            add(item, thd->mem_root);
        }
      }
    }         
  }
  return intersected;
}