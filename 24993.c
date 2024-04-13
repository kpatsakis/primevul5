Item *Item_func_case_simple::find_item()
{
  /* Compare every WHEN argument with it and return the first match */
  uint idx;
  if (!Predicant_to_list_comparator::cmp(this, &idx, NULL))
    return args[idx + when_count()];
  Item **pos= Item_func_case_simple::else_expr_addr();
  return pos ? pos[0] : 0;
}