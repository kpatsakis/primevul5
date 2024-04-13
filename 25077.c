Item_func_nullif::native_op(THD *thd, Native *to)
{
  DBUG_ASSERT(fixed == 1);
  if (!compare())
    return (null_value= true);
  return val_native_with_conversion_from_item(thd, args[2], to, type_handler());
}