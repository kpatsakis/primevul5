bool JOIN::check_two_phase_optimization(THD *thd)
{
  if (check_for_splittable_materialized())
    return true;
  return false;
}