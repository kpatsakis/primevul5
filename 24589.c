Item_func_nullif::time_op(THD *thd, MYSQL_TIME *ltime)
{
  DBUG_ASSERT(fixed == 1);
  if (!compare())
    return (null_value= true);
  return (null_value= Time(thd, args[2]).copy_to_mysql_time(ltime));

}