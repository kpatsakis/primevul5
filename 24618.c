bool Item_func_interval::fix_fields(THD *thd, Item **ref)
{
  if (Item_long_func::fix_fields(thd, ref))
    return true;
  for (uint i= 0 ; i < row->cols(); i++)
  {
    if (row->element_index(i)->check_cols(1))
      return true;
  }
  return false;
}