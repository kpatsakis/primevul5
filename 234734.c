cp_buffer_from_ref(THD *thd, TABLE *table, TABLE_REF *ref)
{
  Check_level_instant_set check_level_save(thd, CHECK_FIELD_IGNORE);
  MY_BITMAP *old_map= dbug_tmp_use_all_columns(table, &table->write_set);
  bool result= 0;

  for (store_key **copy=ref->key_copy ; *copy ; copy++)
  {
    if ((*copy)->copy() & 1)
    {
      result= 1;
      break;
    }
  }
  dbug_tmp_restore_column_map(&table->write_set, old_map);
  return result;
}