  virtual bool walk(Item_processor processor, bool walk_subquery, void *arg)
  {
    return (this->*processor)(arg);
  }