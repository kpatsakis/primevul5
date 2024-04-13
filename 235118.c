  bool eval_const_cond()
  {
    DBUG_ASSERT(const_item());
    DBUG_ASSERT(!is_expensive());
    return val_bool();
  }