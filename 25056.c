bool Predicant_to_list_comparator::add_value(const char *funcname,
                                             Item_args *args,
                                             uint value_index)
{
  DBUG_ASSERT(m_predicant_index < args->argument_count());
  DBUG_ASSERT(value_index < args->argument_count());
  Type_handler_hybrid_field_type tmp;
  Item *tmpargs[2];
  tmpargs[0]= args->arguments()[m_predicant_index];
  tmpargs[1]= args->arguments()[value_index];
  if (tmp.aggregate_for_comparison(funcname, tmpargs, 2, true))
  {
    DBUG_ASSERT(current_thd->is_error());
    return true;
  }
  m_comparators[m_comparator_count].m_handler= tmp.type_handler();
  m_comparators[m_comparator_count].m_arg_index= value_index;
  m_comparator_count++;
  return false;
}