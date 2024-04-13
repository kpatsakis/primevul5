AGGR_OP::prepare_tmp_table()
{
  TABLE *table= join_tab->table;
  JOIN *join= join_tab->join;
  int rc= 0;

  if (!join_tab->table->is_created())
  {
    if (instantiate_tmp_table(table, join_tab->tmp_table_param->keyinfo,
                              join_tab->tmp_table_param->start_recinfo,
                              &join_tab->tmp_table_param->recinfo,
                              join->select_options))
      return true;
    (void) table->file->extra(HA_EXTRA_WRITE_CACHE);
    empty_record(table);
  }
  /* If it wasn't already, start index scan for grouping using table index. */
  if (!table->file->inited && table->group &&
      join_tab->tmp_table_param->sum_func_count && table->s->keys)
    rc= table->file->ha_index_init(0, 0);
  else
  {
    /* Start index scan in scanning mode */
    rc= table->file->ha_rnd_init(true);
  }
  if (rc)
  {
    table->file->print_error(rc, MYF(0));
    return true;
  }
  return false;
}