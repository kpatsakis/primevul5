int join_read_key2(THD *thd, JOIN_TAB *tab, TABLE *table, TABLE_REF *table_ref)
{
  int error;
  if (!table->file->inited)
  {
    error= table->file->ha_index_init(table_ref->key, tab ? tab->sorted : TRUE);
    if (error)
    {
      (void) report_error(table, error);
      return 1;
    }
  }

  /*
    The following is needed when one makes ref (or eq_ref) access from row
    comparisons: one must call row->bring_value() to get the new values.
  */
  if (tab && tab->bush_children)
  {
    TABLE_LIST *emb_sj_nest= tab->bush_children->start->emb_sj_nest;
    emb_sj_nest->sj_subq_pred->left_expr->bring_value();
  }

  /* TODO: Why don't we do "Late NULLs Filtering" here? */

  if (cmp_buffer_with_ref(thd, table, table_ref) ||
      (table->status & (STATUS_GARBAGE | STATUS_NO_PARENT | STATUS_NULL_ROW)))
  {
    if (table_ref->key_err)
    {
      table->status=STATUS_NOT_FOUND;
      return -1;
    }
    /*
      Moving away from the current record. Unlock the row
      in the handler if it did not match the partial WHERE.
    */
    if (tab && tab->ref.has_record && tab->ref.use_count == 0)
    {
      tab->read_record.table->file->unlock_row();
      table_ref->has_record= FALSE;
    }
    error=table->file->ha_index_read_map(table->record[0],
                                  table_ref->key_buff,
                                  make_prev_keypart_map(table_ref->key_parts),
                                  HA_READ_KEY_EXACT);
    if (error && error != HA_ERR_KEY_NOT_FOUND && error != HA_ERR_END_OF_FILE)
      return report_error(table, error);

    if (! error)
    {
      table_ref->has_record= TRUE;
      table_ref->use_count= 1;
    }
  }
  else if (table->status == 0)
  {
    DBUG_ASSERT(table_ref->has_record);
    table_ref->use_count++;
  }
  table->null_row=0;
  return table->status ? -1 : 0;
}