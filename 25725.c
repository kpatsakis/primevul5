bool TABLE::vers_check_update(List<Item> &items)
{
  List_iterator<Item> it(items);
  if (!versioned_write())
    return false;

  while (Item *item= it++)
  {
    if (Item_field *item_field= item->field_for_view_update())
    {
      Field *field= item_field->field;
      if (field->table == this && !field->vers_update_unversioned())
      {
        no_cache= true;
        return true;
      }
    }
  }
  return false;
}