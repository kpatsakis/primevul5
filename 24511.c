ha_rows ha_maria::multi_range_read_info(uint keyno, uint n_ranges, uint keys,
                                       uint key_parts, uint *bufsz,
                                       uint *flags, Cost_estimate *cost)
{
  ds_mrr.init(this, table);
  return ds_mrr.dsmrr_info(keyno, n_ranges, keys, key_parts, bufsz, flags, cost);
}