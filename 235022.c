  bool walk(Item_processor processor, bool walk_subquery, void *arg)
  { 
    return (*ref)->walk(processor, walk_subquery, arg) ||
           (this->*processor)(arg);
  }