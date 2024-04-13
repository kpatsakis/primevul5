static bool check_fields(THD *thd, List<Item> &items, bool update_view)
{
  Item *item;
  if (update_view)
  {
    List_iterator<Item> it(items);
    Item_field *field;
    while ((item= it++))
    {
      if (!(field= item->field_for_view_update()))
      {
        /* item has name, because it comes from VIEW SELECT list */
        my_error(ER_NONUPDATEABLE_COLUMN, MYF(0), item->name.str);
        return TRUE;
      }
      /*
        we make temporary copy of Item_field, to avoid influence of changing
        result_field on Item_ref which refer on this field
      */
      thd->change_item_tree(it.ref(),
                            new (thd->mem_root) Item_field(thd, field));
    }
  }

  if (thd->variables.sql_mode & MODE_SIMULTANEOUS_ASSIGNMENT)
  {
    // Make sure that a column is updated only once
    List_iterator_fast<Item> it(items);
    while ((item= it++))
    {
      item->field_for_view_update()->field->clear_has_explicit_value();
    }
    it.rewind();
    while ((item= it++))
    {
      Field *f= item->field_for_view_update()->field;
      if (f->has_explicit_value())
      {
        my_error(ER_UPDATED_COLUMN_ONLY_ONCE, MYF(0),
                 *(f->table_name), f->field_name.str);
        return TRUE;
      }
      f->set_has_explicit_value();
    }
  }
  return FALSE;
}