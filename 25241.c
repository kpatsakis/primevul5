bool Arg_comparator::set_cmp_func_row()
{
  func= is_owner_equal_func() ? &Arg_comparator::compare_e_row :
                                &Arg_comparator::compare_row;
  return set_cmp_func_for_row_arguments();
}