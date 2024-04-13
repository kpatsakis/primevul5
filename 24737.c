bool Item_func_nullif::walk(Item_processor processor,
                            bool walk_subquery, void *arg)
{
  /*
    No needs to iterate through args[2] when it's just a copy of args[0].
    See MDEV-9712 Performance degradation of nested NULLIF
  */
  uint tmp_count= arg_count == 2 || args[0] == args[2] ? 2 : 3;
  for (uint i= 0; i < tmp_count; i++)
  {
    if (args[i]->walk(processor, walk_subquery, arg))
      return true;
  }
  return (this->*processor)(arg);
}