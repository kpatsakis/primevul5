Item_func_nullif::date_op(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate)
{
  DBUG_ASSERT(fixed == 1);
  if (!compare())
    return (null_value= true);
  Datetime_truncation_not_needed dt(thd, args[2], fuzzydate);
  return (null_value= dt.copy_to_mysql_time(ltime, mysql_timestamp_type()));
}