  void make_sort_key(uchar *to, Item *item, const SORT_FIELD_ATTR *sort_field,
                     Sort_param *param) const
  {
    type_handler()->make_sort_key(to, item, sort_field, param);
  }