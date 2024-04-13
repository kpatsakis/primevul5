bool cond_is_datetime_is_null(Item *cond)
{
  if (cond->type() == Item::FUNC_ITEM &&
      ((Item_func*) cond)->functype() == Item_func::ISNULL_FUNC)
  {
    return ((Item_func_isnull*) cond)->arg_is_datetime_notnull_field();
  }
  return false;
}