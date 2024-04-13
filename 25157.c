in_longlong::in_longlong(THD *thd, uint elements)
  :in_vector(thd, elements, sizeof(packed_longlong),
             (qsort2_cmp) cmp_longlong, 0)
{}