bool Item_func_case::native_op(THD *thd, Native *to)
{
  DBUG_ASSERT(fixed == 1);
  Item *item= find_item();
  if (!item)
    return (null_value= true);
  return val_native_with_conversion_from_item(thd, item, to, type_handler());
}