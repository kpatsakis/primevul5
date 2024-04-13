int ha_maria::multi_range_read_explain_info(uint mrr_mode, char *str,
                                            size_t size)
{
  return ds_mrr.dsmrr_explain_info(mrr_mode, str, size);
}