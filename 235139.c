  longlong val_int_from_item(Item *item)
  {
    DBUG_ASSERT(fixed == 1);
    longlong value= item->val_int();
    null_value= item->null_value;
    return value;
  }