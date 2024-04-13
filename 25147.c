bool Item_func_ifnull::native_op(THD *thd, Native *to)
{
  DBUG_ASSERT(fixed == 1);
  if (!val_native_with_conversion_from_item(thd, args[0], to, type_handler()))
    return false;
  return val_native_with_conversion_from_item(thd, args[1], to, type_handler());
}