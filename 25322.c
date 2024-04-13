bool Item_func_coalesce::native_op(THD *thd, Native *to)
{
  DBUG_ASSERT(fixed == 1);
  for (uint i= 0; i < arg_count; i++)
  {
    if (!val_native_with_conversion_from_item(thd, args[i], to, type_handler()))
      return false;
  }
  return (null_value= true);
}