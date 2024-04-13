  longlong val_int_from_item(Item *item)
  {
    DBUG_ASSERT(is_fixed());
    longlong value= item->val_int();
    null_value= item->null_value;
    return value;
  }