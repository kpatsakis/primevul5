void Item_in_optimizer::restore_first_argument()
{
  if (!invisible_mode())
  {
    args[0]= ((Item_in_subselect *)args[1])->left_expr;
  }
}