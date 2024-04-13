  With_sum_func_cache(const Item *a, const Item *b, const Item *c)
   :m_with_sum_func(a->with_sum_func() || b->with_sum_func() ||
                    c->with_sum_func())
  { }