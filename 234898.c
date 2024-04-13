  Item_func_or_sum(THD *thd, List<Item> &list):
    Item_result_field(thd), Item_args(thd, list) { }