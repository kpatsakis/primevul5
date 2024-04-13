  bool walk(Item_processor processor, bool walk_subquery, void *arg)
  {
    if (walk_args(processor, walk_subquery, arg))
      return true;
    return (this->*processor)(arg);
  }