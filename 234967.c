  static Item_cache* get_cache(THD *thd, const Item* item,
                         const Item_result type)
  {
    return get_cache(thd, item, type, item->field_type());
  }