int ha_maria::index_end()
{
  active_index=MAX_KEY;
  ma_set_index_cond_func(file, NULL, 0);
  in_range_check_pushed_down= FALSE;
  ds_mrr.dsmrr_close();
  return 0;
}