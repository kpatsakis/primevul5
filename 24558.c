int ha_maria::index_read_last_map(uchar * buf, const uchar * key,
				  key_part_map keypart_map)
{
  DBUG_ENTER("ha_maria::index_read_last_map");
  DBUG_ASSERT(inited == INDEX);
  register_handler(file);
  int error= maria_rkey(file, buf, active_index, key, keypart_map,
                        HA_READ_PREFIX_LAST);
  DBUG_RETURN(error);
}