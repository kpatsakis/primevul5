  Item_func_or_sum(THD *thd, Item *a, Item *b, Item *c):
    Item_result_field(thd), Item_args(thd, a, b, c) { }