int ha_maria::index_read_idx_map(uchar * buf, uint index, const uchar * key,
				 key_part_map keypart_map,
				 enum ha_rkey_function find_flag)
{
  int error;
  register_handler(file);

  /* Use the pushed index condition if it matches the index we're scanning */
  end_range= NULL;
  if (index == pushed_idx_cond_keyno)
    ma_set_index_cond_func(file, handler_index_cond_check, this);

  error= maria_rkey(file, buf, index, key, keypart_map, find_flag);

  ma_set_index_cond_func(file, NULL, 0);
  return error;
}