  double val_real_from_item(Item *item)
  {
    DBUG_ASSERT(is_fixed());
    double value= item->val_real();
    null_value= item->null_value;
    return value;
  }