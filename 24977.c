  bool save_in_value(THD *thd, struct st_value *value)
  {
    return type_handler()->Item_save_in_value(thd, this, value);
  }