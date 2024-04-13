bool Virtual_tmp_table::sp_set_all_fields_from_item_list(THD *thd,
                                                         List<Item> &items)
{
  DBUG_ASSERT(s->fields == items.elements);
  List_iterator<Item> it(items);
  Item *item;
  for (uint i= 0 ; (item= it++) ; i++)
  {
    if (field[i]->sp_prepare_and_store_item(thd, &item))
      return true;
  }
  return false;
}