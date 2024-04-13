  bool is_expensive_processor(void *arg)
  {
    DBUG_ASSERT(example);
    if (value_cached)
      return false;
    return example->is_expensive_processor(arg);
  }