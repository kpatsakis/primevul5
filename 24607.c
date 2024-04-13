  With_sum_func_cache(const Item *a, const Item *b)
   :m_with_sum_func(a->with_sum_func() || b->with_sum_func())
  { }