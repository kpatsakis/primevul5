bool Item_func_case_simple::prepare_predicant_and_values(THD *thd,
                                                         uint *found_types,
                                                         bool nulls_equal)
{
  bool have_null= false;
  uint type_cnt;
  Type_handler_hybrid_field_type tmp;
  uint ncases= when_count();
  add_predicant(this, 0);
  for (uint i= 0 ; i < ncases; i++)
  {
    if (nulls_equal ?
        add_value("case..when", this, i + 1) :
        add_value_skip_null("case..when", this, i + 1, &have_null))
      return true;
  }
  all_values_added(&tmp, &type_cnt, &m_found_types);
#ifndef DBUG_OFF
  Predicant_to_list_comparator::debug_print(thd);
#endif
  return false;
}