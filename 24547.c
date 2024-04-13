const char *ha_maria::index_type(uint key_number)
{
  return ((table->key_info[key_number].flags & HA_FULLTEXT) ?
          "FULLTEXT" :
          (table->key_info[key_number].flags & HA_SPATIAL) ?
          "SPATIAL" :
          (table->key_info[key_number].algorithm == HA_KEY_ALG_RTREE) ?
          "RTREE" : "BTREE");
}