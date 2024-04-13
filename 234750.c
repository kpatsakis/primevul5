end_write(JOIN *join, JOIN_TAB *join_tab __attribute__((unused)),
	  bool end_of_records)
{
  TABLE *const table= join_tab->table;
  DBUG_ENTER("end_write");

  if (!end_of_records)
  {
    copy_fields(join_tab->tmp_table_param);
    if (copy_funcs(join_tab->tmp_table_param->items_to_copy, join->thd))
      DBUG_RETURN(NESTED_LOOP_ERROR);           /* purecov: inspected */

    if (likely(!join_tab->having || join_tab->having->val_int()))
    {
      int error;
      join->found_records++;
      if ((error= table->file->ha_write_tmp_row(table->record[0])))
      {
        if (likely(!table->file->is_fatal_error(error, HA_CHECK_DUP)))
	  goto end;                             // Ignore duplicate keys
        bool is_duplicate;
	if (create_internal_tmp_table_from_heap(join->thd, table, 
                                                join_tab->tmp_table_param->start_recinfo,
                                                &join_tab->tmp_table_param->recinfo,
                                                error, 1, &is_duplicate))
	  DBUG_RETURN(NESTED_LOOP_ERROR);        // Not a table_is_full error
        if (is_duplicate)
          goto end;
	table->s->uniques=0;			// To ensure rows are the same
      }
      if (++join_tab->send_records >=
            join_tab->tmp_table_param->end_write_records &&
	  join->do_send_rows)
      {
	if (!(join->select_options & OPTION_FOUND_ROWS))
	  DBUG_RETURN(NESTED_LOOP_QUERY_LIMIT);
	join->do_send_rows=0;
	join->unit->select_limit_cnt = HA_POS_ERROR;
      }
    }
  }
end:
  if (unlikely(join->thd->check_killed()))
  {
    DBUG_RETURN(NESTED_LOOP_KILLED);             /* purecov: inspected */
  }
  DBUG_RETURN(NESTED_LOOP_OK);
}