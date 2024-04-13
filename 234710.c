bool cond_has_datetime_is_null(Item *cond)
{
  if (cond_is_datetime_is_null(cond))
    return true;

  if (cond->type() == Item::COND_ITEM)
  {
    List<Item> *cond_arg_list= ((Item_cond*) cond)->argument_list();
    List_iterator<Item> li(*cond_arg_list);
    Item *item;
    while ((item= li++))
    {
      if (cond_has_datetime_is_null(item))
        return true;
    }
  }
  return false;
}