  bool val_native(THD *thd, Native *to)
  {
    return Item_param::type_handler()->Item_param_val_native(thd, this, to);
  }