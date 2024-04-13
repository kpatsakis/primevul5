in_decimal::in_decimal(THD *thd, uint elements)
  :in_vector(thd, elements, sizeof(my_decimal), (qsort2_cmp) cmp_decimal, 0)
{}