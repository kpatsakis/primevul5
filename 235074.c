  Item *get_tmp_table_item(THD *thd)
  {
    if (const_item())
      return copy_or_same(thd);
    Item *item= Item_ref::get_tmp_table_item(thd);
    item->name= name;
    return item;
  }