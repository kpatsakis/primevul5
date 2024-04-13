  virtual bool is_expensive()
  {
    if (is_expensive_cache < 0)
      is_expensive_cache= walk(&Item::is_expensive_processor, 0, NULL);
    return MY_TEST(is_expensive_cache);
  }