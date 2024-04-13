int ha_maria::index_read_map(uchar * buf, const uchar * key,
			     key_part_map keypart_map,
			     enum ha_rkey_function find_flag)
{
  DBUG_ASSERT(inited == INDEX);
  register_handler(file);
  int error= maria_rkey(file, buf, active_index, key, keypart_map, find_flag);
  return error;
}