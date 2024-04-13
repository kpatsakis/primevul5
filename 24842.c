  my_decimal *val_decimal_from_item(Item *item, my_decimal *decimal_value)
  {
    DBUG_ASSERT(is_fixed());
    my_decimal *value= item->val_decimal(decimal_value);
    if ((null_value= item->null_value))
      value= NULL;
    return value;
  }