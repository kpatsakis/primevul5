  double val_real_from_item(Item *item)
  {
    DBUG_ASSERT(fixed == 1);
    double value= item->val_real();
    null_value= item->null_value;
    return value;
  }