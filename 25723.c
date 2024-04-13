static bool safe_update_on_fly(THD *thd, JOIN_TAB *join_tab,
                               TABLE_LIST *table_ref, TABLE_LIST *all_tables)
{
  TABLE *table= join_tab->table;
  if (unique_table(thd, table_ref, all_tables, 0))
    return 0;
  if (join_tab->join->order) // FIXME this is probably too strong
    return 0;
  switch (join_tab->type) {
  case JT_SYSTEM:
  case JT_CONST:
  case JT_EQ_REF:
    return TRUE;				// At most one matching row
  case JT_REF:
  case JT_REF_OR_NULL:
    return !is_key_used(table, join_tab->ref.key, table->write_set);
  case JT_ALL:
    if (bitmap_is_overlapping(&table->tmp_set, table->write_set))
      return FALSE;
    /* If range search on index */
    if (join_tab->quick)
      return !join_tab->quick->is_keys_used(table->write_set);
    /* If scanning in clustered key */
    if ((table->file->ha_table_flags() & HA_PRIMARY_KEY_IN_READ_INDEX) &&
	table->s->primary_key < MAX_KEY)
      return !is_key_used(table, table->s->primary_key, table->write_set);
    return TRUE;
  default:
    break;					// Avoid compiler warning
  }
  return FALSE;

}