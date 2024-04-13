void setup_defaults(THD *thd, List<Item> &fields, List<Item> &values)
{
  List_iterator<Item> fit(fields);
  List_iterator<Item> vit(values);

  for (Item *value= vit++, *f_item= fit++; value; value= vit++, f_item= fit++)
  {
    value->walk(&Item::enchant_default_with_arg_processor, false, f_item);
  }
}