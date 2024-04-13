bool Item_func_case_simple::fix_length_and_dec()
{
  THD *thd= current_thd;
  return (aggregate_then_and_else_arguments(thd, when_count() + 1) ||
          aggregate_switch_and_when_arguments(thd, false));
}