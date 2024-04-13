  Item_func_or_sum(THD *thd, Item_func_or_sum *item):
    Item_result_field(thd, item), Item_args(thd, item),
    Used_tables_and_const_cache(item) { }