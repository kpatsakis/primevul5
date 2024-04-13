  static Item_cache* get_cache(THD *thd, const Item *item)
  {
    return get_cache(thd, item, item->cmp_type());
  }