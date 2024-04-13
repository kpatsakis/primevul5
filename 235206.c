  bool walk_args(Item_processor processor, bool walk_subquery, void *arg)
  {
    for (uint i= 0; i < arg_count; i++)
    {
      if (args[i]->walk(processor, walk_subquery, arg))
        return true;
    }
    return false;
  }