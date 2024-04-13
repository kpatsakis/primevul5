  bool val_native_with_conversion_from_item(THD *thd, Item *item, Native *to,
                                            const Type_handler *handler)
  {
    DBUG_ASSERT(is_fixed());
    return null_value= item->val_native_with_conversion(thd, to, handler);
  }