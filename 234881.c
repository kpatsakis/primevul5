  void sortlength(THD *thd,
                  const Type_std_attributes *item,
                  SORT_FIELD_ATTR *attr) const
  {
    type_handler()->sortlength(thd, item, attr);
  }