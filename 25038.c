longlong Item_func_isnull::val_int()
{
  DBUG_ASSERT(fixed == 1);
  if (const_item() && !args[0]->maybe_null)
    return 0;
  return args[0]->is_null() ? 1: 0;
}