longlong Item_func_truth::val_int()
{
  return (val_bool() ? 1 : 0);
}