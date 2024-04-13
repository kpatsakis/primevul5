void Item_func_in::fix_in_vector()
{
  DBUG_ASSERT(array);
  uint j=0;
  for (uint i=1 ; i < arg_count ; i++)
  {
    array->set(j,args[i]);
    if (!args[i]->null_value)
      j++; // include this cell in the array.
    else
    {
      /*
        We don't put NULL values in array, to avoid erronous matches in
        bisection.
      */
      have_null= 1;
    }
  }
  if ((array->used_count= j))
    array->sort();
}