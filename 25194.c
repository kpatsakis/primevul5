  Item *remove_item_direct_ref()
  {
    *ref= (*ref)->remove_item_direct_ref();
    return this;
  }