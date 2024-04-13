int ha_maria::index_init(uint idx, bool sorted)
{
  active_index=idx;
  if (pushed_idx_cond_keyno == idx)
    ma_set_index_cond_func(file, handler_index_cond_check, this);
  return 0;
}