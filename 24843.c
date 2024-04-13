  bool walk(Item_processor processor, bool walk_subquery, void *arg)
  {
    if (arg == STOP_PTR)
      return FALSE;
    if (example && example->walk(processor, walk_subquery, arg))
      return TRUE;
    return (this->*processor)(arg);
  }