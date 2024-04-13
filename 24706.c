Item *Item_func_decode_oracle::find_item()
{
  uint idx;
  if (!Predicant_to_list_comparator::cmp_nulls_equal(current_thd, this, &idx))
    return args[idx + when_count()];
  Item **pos= Item_func_decode_oracle::else_expr_addr();
  return pos ? pos[0] : 0;
}