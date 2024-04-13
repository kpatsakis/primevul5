int Arg_comparator::set_cmp_func(Item_func_or_sum *owner_arg,
                                 Item **a1, Item **a2)
{
  owner= owner_arg;
  set_null= set_null && owner_arg;
  a= a1;
  b= a2;
  Item *tmp_args[2]= {*a1, *a2};
  Type_handler_hybrid_field_type tmp;
  if (tmp.aggregate_for_comparison(owner_arg->func_name(), tmp_args, 2, false))
  {
    DBUG_ASSERT(current_thd->is_error());
    return 1;
  }
  m_compare_handler= tmp.type_handler();
  return m_compare_handler->set_comparator_func(this);
}