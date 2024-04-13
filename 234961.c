  Item_cache_row(THD *thd):
    Item_cache(thd), values(0), item_count(2),
    save_array(0) {}