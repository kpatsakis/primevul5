in_double::in_double(THD *thd, uint elements)
  :in_vector(thd, elements, sizeof(double), (qsort2_cmp) cmp_double, 0)
{}