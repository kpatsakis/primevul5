in_string::in_string(THD *thd, uint elements, qsort2_cmp cmp_func,
                     CHARSET_INFO *cs)
  :in_vector(thd, elements, sizeof(String), cmp_func, cs),
   tmp(buff, sizeof(buff), &my_charset_bin)
{}