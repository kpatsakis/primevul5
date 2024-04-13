longlong Item_func_xor::val_int()
{
  DBUG_ASSERT(fixed == 1);
  int result= 0;
  null_value= false;
  for (uint i= 0; i < arg_count; i++)
  {
    result^= (args[i]->val_int() != 0);
    if (args[i]->null_value)
    {
      null_value= true;
      return 0;
    }
  }
  return result;
}