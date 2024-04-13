Item *Item_func_case_searched::find_item()
{
  uint count= when_count();
  for (uint i= 0 ; i < count ; i++)
  {
    if (args[i]->val_bool())
      return args[i + count];
  }
  Item **pos= Item_func_case_searched::else_expr_addr();
  return pos ? pos[0] : 0;
}