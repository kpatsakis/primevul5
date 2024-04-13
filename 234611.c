join_read_always_key(JOIN_TAB *tab)
{
  int error;
  TABLE *table= tab->table;

  /* Initialize the index first */
  if (!table->file->inited)
  {
    if (unlikely((error= table->file->ha_index_init(tab->ref.key,
                                                    tab->sorted))))
    {
      (void) report_error(table, error);
      return 1;
    }
  }

  if (unlikely(cp_buffer_from_ref(tab->join->thd, table, &tab->ref)))
    return -1;
  if (unlikely((error=
                table->file->prepare_index_key_scan_map(tab->ref.key_buff,
                                                        make_prev_keypart_map(tab->ref.key_parts)))))
  {
    report_error(table,error);
    return -1;
  }
  if ((error= table->file->ha_index_read_map(table->record[0],
                                             tab->ref.key_buff,
                                             make_prev_keypart_map(tab->ref.key_parts),
                                             HA_READ_KEY_EXACT)))
  {
    if (error != HA_ERR_KEY_NOT_FOUND && error != HA_ERR_END_OF_FILE)
      return report_error(table, error);
    return -1; /* purecov: inspected */
  }
  return 0;
}