  Item_result_field(THD *thd, Item_result_field *item):
    Item_fixed_hybrid(thd, item), result_field(item->result_field)
  {}