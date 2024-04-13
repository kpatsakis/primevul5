bool Item_func_in::prepare_predicant_and_values(THD *thd, uint *found_types)
{
  uint type_cnt;
  have_null= false;

  add_predicant(this, 0);
  for (uint i= 1 ; i < arg_count; i++)
  {
    if (add_value_skip_null(Item_func_in::func_name(), this, i, &have_null))
      return true;
  }
  all_values_added(&m_comparator, &type_cnt, found_types);
  arg_types_compatible= type_cnt < 2;

#ifndef DBUG_OFF
  Predicant_to_list_comparator::debug_print(thd);
#endif
  return false;
}