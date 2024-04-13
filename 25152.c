void Item_func_case::reorder_args(uint start)
{
  /*
    Reorder args, to have at first the optional CASE expression, then all WHEN
    expressions, then all THEN expressions. And the optional ELSE expression
    at the end.

    We reorder an even number of arguments, starting from start.
  */
  uint count = (arg_count - start) / 2;
  const size_t size= sizeof(Item*) * count * 2;
  Item **arg_buffer= (Item **)my_safe_alloca(size);
  memcpy(arg_buffer, &args[start], size);
  for (uint i= 0; i < count; i++)
  {
    args[start + i]= arg_buffer[i*2];
    args[start + i + count]= arg_buffer[i*2 + 1];
  }
  my_safe_afree(arg_buffer, size);
}