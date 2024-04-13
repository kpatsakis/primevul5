void ha_maria::change_table_ptr(TABLE *table_arg, TABLE_SHARE *share)
{
  handler::change_table_ptr(table_arg, share);
  if (file)
    file->external_ref= table_arg;
}