int JOIN::optimize()
{
  int res= 0;
  create_explain_query_if_not_exists(thd->lex, thd->mem_root);
  join_optimization_state init_state= optimization_state;
  if (optimization_state == JOIN::OPTIMIZATION_PHASE_1_DONE)
    res= optimize_stage2();
  else
  {
    // to prevent double initialization on EXPLAIN
    if (optimization_state != JOIN::NOT_OPTIMIZED)
      return FALSE;
    optimization_state= JOIN::OPTIMIZATION_IN_PROGRESS;
    res= optimize_inner();
  }
  if (!with_two_phase_optimization ||
      init_state == JOIN::OPTIMIZATION_PHASE_1_DONE)
  {
    if (!res && have_query_plan != QEP_DELETED)
      res= build_explain();
    optimization_state= JOIN::OPTIMIZATION_DONE;
  }
  return res;
}