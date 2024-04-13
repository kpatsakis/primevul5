  Item_func_or_sum(THD *thd, Item *a, Item *b):
    Item_result_field(thd), Item_args(a, b) { }