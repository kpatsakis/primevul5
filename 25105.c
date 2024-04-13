bool Item_func_case::time_op(THD *thd, MYSQL_TIME *ltime)
{
  DBUG_ASSERT(fixed == 1);
  Item *item= find_item();
  if (!item)
    return (null_value= true);
  return (null_value= Time(thd, item).copy_to_mysql_time(ltime));
}