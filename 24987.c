  With_sum_func_cache(const Item *a, const Item *b, const Item *c,
                      const Item *d, const Item *e)
   :m_with_sum_func(a->with_sum_func() || b->with_sum_func() ||
                    c->with_sum_func() || d->with_sum_func() ||
                    e->with_sum_func())
  { }