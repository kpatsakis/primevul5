find_field_in_item_list (Field *field, void *data)
{
  List<Item> *fields= (List<Item> *) data;
  bool part_found= 0;
  List_iterator<Item> li(*fields);
  Item *item;

  while ((item= li++))
  {
    if (item->real_item()->type() == Item::FIELD_ITEM &&
	((Item_field*) (item->real_item()))->field->eq(field))
    {
      part_found= 1;
      break;
    }
  }
  return part_found;
}