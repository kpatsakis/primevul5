  Item_cache_int(THD *thd, enum_field_types field_type_arg):
    Item_cache(thd, field_type_arg), value(0) {}