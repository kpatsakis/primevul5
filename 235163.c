  bool walk(Item_processor processor, bool walk_subquery, void *arg)
  {
    return orig_item->walk(processor, walk_subquery, arg) ||
      (this->*processor)(arg);
  }