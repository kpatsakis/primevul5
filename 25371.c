bool Item_func_coalesce::date_op(THD *thd, MYSQL_TIME *ltime, date_mode_t fuzzydate)
{
  DBUG_ASSERT(fixed == 1);
  for (uint i= 0; i < arg_count; i++)
  {
    Datetime_truncation_not_needed dt(thd, args[i],
                                      fuzzydate & ~TIME_FUZZY_DATES);
    if (!dt.copy_to_mysql_time(ltime, mysql_timestamp_type()))
      return (null_value= false);
  }
  return (null_value= true);
}