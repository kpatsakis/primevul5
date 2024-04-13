ha_rows ha_maria::records_in_range(uint inx, const key_range *min_key,
                                   const key_range *max_key, page_range *pages)
{
  register_handler(file);
  return (ha_rows) maria_records_in_range(file, (int) inx, min_key, max_key,
                                          pages);
}