int _ma_killed_ptr(HA_CHECK *param)
{
  if (likely(thd_killed((THD*)param->thd)) == 0)
    return 0;
  my_errno= HA_ERR_ABORTED_BY_USER;
  return 1;
}