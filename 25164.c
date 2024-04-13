bool Item_func_coalesce::time_op(THD *thd, MYSQL_TIME *ltime)
{
  DBUG_ASSERT(fixed == 1);
  for (uint i= 0; i < arg_count; i++)
  {
    if (!Time(thd, args[i]).copy_to_mysql_time(ltime))
      return (null_value= false);
  }
  return (null_value= true);
}