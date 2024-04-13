  bool val_native_from_item(THD *thd, Item *item, Native *to)
  {
    DBUG_ASSERT(is_fixed());
    null_value= item->val_native(thd, to);
    DBUG_ASSERT(null_value == item->null_value);
    return null_value;
  }