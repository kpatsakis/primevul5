  bool val_native_with_conversion_result(THD *thd, Native *to,
                                         const Type_handler *th)
  {
    return th->Item_val_native_with_conversion_result(thd, this, to);
  }