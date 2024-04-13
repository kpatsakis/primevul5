  Item_cache* get_cache(THD *thd) const
  {
    return type_handler()->Item_get_cache(thd, this);
  }