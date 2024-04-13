bool Item_func_case::date_op(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate)
{
  DBUG_ASSERT(fixed == 1);
  Item *item= find_item();
  if (!item)
    return (null_value= true);
  Datetime_truncation_not_needed dt(thd, item, fuzzydate);
  return (null_value= dt.copy_to_mysql_time(ltime, mysql_timestamp_type()));
}