  bool set_value(THD *thd, const Type_all_attributes *attr,
                 const st_value *val, const Type_handler *h)
  {
    value.set_handler(h); // See comments in set_param_func()
    return h->Item_param_set_from_value(thd, this, attr, val);
  }