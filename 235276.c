  bool eq(const Item *item, bool binary_cmp) const
  {
    Item *it= ((Item *) item)->real_item();
    return orig_item->eq(it, binary_cmp);
  }