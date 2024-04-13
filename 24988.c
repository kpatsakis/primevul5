bool Item_func_case::fix_fields(THD *thd, Item **ref)
{
  bool res= Item_func::fix_fields(thd, ref);

  Item **pos= else_expr_addr();
  if (!pos || pos[0]->maybe_null)
    maybe_null= 1;
  return res;
}