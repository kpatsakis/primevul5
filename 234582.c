int JOIN::init_join_caches()
{
  JOIN_TAB *tab;

  for (tab= first_linear_tab(this, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab;
       tab= next_linear_tab(this, tab, WITH_BUSH_ROOTS))
  {
    TABLE *table= tab->table;
    if (table->file->keyread_enabled())
    {
      if (!(table->file->index_flags(table->file->keyread, 0, 1) & HA_CLUSTERED_INDEX))
        table->mark_index_columns(table->file->keyread, table->read_set);
    }
    else if ((tab->read_first_record == join_read_first ||
              tab->read_first_record == join_read_last) &&
             !tab->filesort && table->covering_keys.is_set(tab->index) &&
             !table->no_keyread)
    {
      table->prepare_for_keyread(tab->index, table->read_set);
    }
    if (tab->cache && tab->cache->init(select_options & SELECT_DESCRIBE))
      revise_cache_usage(tab);
    else
      tab->remove_redundant_bnl_scan_conds();
  }
  return 0;
}