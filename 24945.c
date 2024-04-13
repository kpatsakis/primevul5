bool Item_func_in::fix_length_and_dec()
{
  THD *thd= current_thd;
  uint found_types;
  m_comparator.set_handler(type_handler_varchar.type_handler_for_comparison());
  max_length= 1;

  if (prepare_predicant_and_values(thd, &found_types))
  {
    DBUG_ASSERT(thd->is_error()); // Must set error
    return TRUE;
  }

  if (arg_types_compatible) // Bisection condition #1
  {
    if (m_comparator.type_handler()->
        Item_func_in_fix_comparator_compatible_types(thd, this))
      return TRUE;
  }
  else
  {
    DBUG_ASSERT(m_comparator.cmp_type() != ROW_RESULT);
    if ( fix_for_scalar_comparison_using_cmp_items(thd, found_types))
      return TRUE;
  }

  DBUG_EXECUTE_IF("Item_func_in",
                  push_warning_printf(thd, Sql_condition::WARN_LEVEL_NOTE,
                  ER_UNKNOWN_ERROR, "DBUG: types_compatible=%s bisect=%s",
                  arg_types_compatible ? "yes" : "no",
                  array != NULL ? "yes" : "no"););
  return FALSE;
}