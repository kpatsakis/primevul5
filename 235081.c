  bool walk(Item_processor processor, bool walk_subquery, void *args)
  {
    return (arg && arg->walk(processor, walk_subquery, args)) ||
      (this->*processor)(args);
  }