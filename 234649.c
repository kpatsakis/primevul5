AGGR_OP::put_record(bool end_of_records)
{
  // Lasy tmp table creation/initialization
  if (!join_tab->table->file->inited)
    if (prepare_tmp_table())
      return NESTED_LOOP_ERROR;
  enum_nested_loop_state rc= (*write_func)(join_tab->join, join_tab,
                                           end_of_records);
  return rc;
}