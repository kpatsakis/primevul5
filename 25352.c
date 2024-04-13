  Item_cache_int(THD *thd): Item_cache(thd, &type_handler_longlong),
    value(0) {}