bool Virtual_tmp_table::sp_set_all_fields_from_item(THD *thd, Item *value)
{
  DBUG_ASSERT(value->fixed);
  DBUG_ASSERT(value->cols() == s->fields);
  for (uint i= 0; i < value->cols(); i++)
  {
    if (field[i]->sp_prepare_and_store_item(thd, value->addr(i)))
      return true;
  }
  return false;
}