ulong ha_maria::index_flags(uint inx, uint part, bool all_parts) const
{
  ulong flags;
  if (table_share->key_info[inx].algorithm == HA_KEY_ALG_FULLTEXT)
    flags= 0;
  else
  if ((table_share->key_info[inx].flags & HA_SPATIAL ||
      table_share->key_info[inx].algorithm == HA_KEY_ALG_RTREE))
  {
    /* All GIS scans are non-ROR scans. We also disable IndexConditionPushdown */
    flags= HA_READ_NEXT | HA_READ_PREV | HA_READ_RANGE |
           HA_READ_ORDER | HA_KEYREAD_ONLY | HA_KEY_SCAN_NOT_ROR;
  }
  else
  {
    flags= HA_READ_NEXT | HA_READ_PREV | HA_READ_RANGE |
          HA_READ_ORDER | HA_KEYREAD_ONLY | HA_DO_INDEX_COND_PUSHDOWN;
  }
  return flags;
}