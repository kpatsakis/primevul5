in_timestamp::in_timestamp(THD *thd, uint elements)
  :in_vector(thd, elements, sizeof(Value), (qsort2_cmp) cmp_timestamp, 0)
{}