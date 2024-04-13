longlong Item_equal::val_int()
{
  if (cond_false)
    return 0;
  if (cond_true)
    return 1;
  Item *item= get_const();
  Item_equal_fields_iterator it(*this);
  if (!item)
    item= it++;
  eval_item->store_value(item);
  if ((null_value= item->null_value))
    return 0;
  while ((item= it++))
  {
    Field *field= it.get_curr_field();
    /* Skip fields of tables that has not been read yet */
    if (!field->table->status || (field->table->status & STATUS_NULL_ROW))
    {
      const int rc= eval_item->cmp(item);
      if ((rc == TRUE) || (null_value= (rc == UNKNOWN)))
        return 0;
    }
  }
  return 1;
}